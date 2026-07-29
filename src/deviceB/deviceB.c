/* includes */

#include "deviceB.h"

/* declarations */

static DEVICE_B_INSTANCE    sDeviceB;
static SEMAPHORE_T          xStateDevB;

/* internal prototypes */

static DEVICE_B_STATES_T devB_IfNextState(void);
static ERROR_T devB_IfManageCommunications(void);
static void devB_IfSleepTimerCallback(TIMER_T xTimer);

/* external functions */

void devB_Run(void *pvArgs)
{
    for (;;)
    {
        if (xSemaphoreTake(sDeviceB.xStateDevB, 0) == pdTRUE)
        {
            sDeviceB.eCurrentState = devB_IfNextState();

            xSemaphoreGive(sDeviceB.xStateDevB);
        }

        devB_IfManageCommunications();

        vTaskDelay(pdMS_TO_TICKS(DEVB_TASK_PERIOD));
    }
}

ERROR_T devB_Init()
{
    ERROR_T eValue = E_ERROR;

    sDeviceB.xStateDevB = xSemaphoreCreateMutex();

    if (sDeviceB.xStateDevB != NULL)
    {
        eValue = E_OK;
    }

    if (eValue == E_OK)
    {
        sDeviceB.uTickCounter       = 0;
        sDeviceB.eCurrentState      = DEVB_SLEEP;
        sDeviceB.bResetFlag         = FALSE;
        sDeviceB.bSleepTimerExpired = FALSE;
        sDeviceB.bSleepTimerStarted = FALSE;
        sDeviceB.xSleepTimer        = xTimerCreate( "SleepTimer",
                                                    pdMS_TO_TICKS(5000),   /* 5 seconds */
                                                    pdFALSE,               /* one-shot */
                                                    NULL,
                                                    devB_IfSleepTimerCallback);

        log_Write("[DEVICE_B] initialized");
    }
    else
    {
        log_Write("[DEVICE_B] not initialized");
    }

    return E_OK;
}

ERROR_T devB_GetState(DEVICE_B_STATES_T *peDevBState)
{
    ERROR_T eValue = E_ERROR;

    if (peDevBState != NULL)
    {
        eValue = E_OK;
    }

    if (eValue == E_OK)
    {
        if (xSemaphoreTake(sDeviceB.xStateDevB, 0) == pdTRUE)
        {
            *peDevBState = sDeviceB.eCurrentState;

            xSemaphoreGive(sDeviceB.xStateDevB);
        }
        else
        {
            *peDevBState = DEVB_UNDEFINED;
        }
    }

    return eValue;
}

/* internal functions */

/**
 * @brief Compute the next state of Device B based on timing, sleep timer,
 *        and reset conditions.
 *
 * Behavior:
 * - Increments the internal tick counter.
 * - Transitions to DEVB_FAULT when the counter reaches DEVB_FAULT_TICKS.
 * - Transitions to DEVB_SLEEP when the counter reaches DEVB_SLEEP_TICKS.
 *
 * - DEVB_SLEEP:
 *      • If the sleep timer has expired → transition to DEVB_ACTIVE.
 *      • If the sleep timer is not running → start it.
 *      • Otherwise remain in DEVB_SLEEP.
 *
 * - DEVB_ACTIVE:
 *      • Remains in DEVB_ACTIVE unless overridden by timing conditions.
 *
 * - DEVB_FAULT:
 *      • If reset flag is set → transition to DEVB_SLEEP and clear flag.
 *      • Otherwise remain in DEVB_FAULT.
 *
 * - Any state change is logged.
 *
 * @return The next state of Device B.
 */

static DEVICE_B_STATES_T devB_IfNextState(void)
{
    DEVICE_B_STATES_T eRetState = DEVB_UNDEFINED;

    sDeviceB.uTickCounter++;

    if (sDeviceB.uTickCounter == DEVB_FAULT_TICKS)
    {
        eRetState = DEVB_FAULT;
    }
    else if (sDeviceB.uTickCounter == DEVB_SLEEP_TICKS)
    {
        sDeviceB.uTickCounter = 0;

        eRetState = DEVB_SLEEP;
    }
    else
    {
        switch (sDeviceB.eCurrentState)
        {
            case DEVB_SLEEP:

                if (sDeviceB.bSleepTimerExpired == TRUE)
                {
                    eRetState = DEVB_ACTIVE;

                    sDeviceB.bSleepTimerExpired = FALSE;
                    sDeviceB.bSleepTimerStarted = FALSE;

                    log_Write("[DEVICE_B] sleep timer expired");
                }
                else
                {
                    if (sDeviceB.bSleepTimerStarted == FALSE)
                    {
                        if (xTimerStart(sDeviceB.xSleepTimer, 0) == pdPASS)
                        {
                            sDeviceB.bSleepTimerExpired = FALSE;
                            sDeviceB.bSleepTimerStarted = TRUE;

                            log_Write("[DEVICE_B] sleep timer started");
                        }
                        else
                        {
                            log_Write("[DEVICE_B] cannot start sleep timer");
                        }
                    }

                    eRetState = DEVB_SLEEP;
                }

                break;

            case DEVB_ACTIVE:

                eRetState = DEVB_ACTIVE;

                break;

            case DEVB_FAULT:

                if (sDeviceB.bResetFlag == TRUE)
                {
                    eRetState = DEVB_SLEEP;

                    sDeviceB.uTickCounter = 0;

                    sDeviceB.bResetFlag = FALSE;
                }
                else
                {
                    eRetState = DEVB_FAULT;
                }

                break;

            default:

                eRetState = DEVB_FAULT;

                break;
        }
    }

    if (eRetState != sDeviceB.eCurrentState)
    {
        log_Write("[DEVICE_B] transition %s => %s",
                  DEVICE_B_STATE_STR[sDeviceB.eCurrentState],
                  DEVICE_B_STATE_STR[eRetState]);
    }

    return eRetState;
}

/**
 * @brief Handle asynchronous commands sent by Device A via task notifications.
 *
 * Behavior:
 * - Performs a non-blocking wait for a notification.
 * - If a notification is received, interprets it as a command.
 *
 * Supported commands:
 * - CMD_RESET:
 *      • Sets the reset flag.
 *      • Causes the next state update to transition from FAULT → SLEEP.
 *      • Logs the event.
 *
 * Unsupported commands:
 *      • Logged as errors.
 *      • Returns E_ERROR.
 *
 * If no command is pending, returns E_OK without modifying state.
 *
 * @return E_OK if command is valid or no command is received,
 *         E_ERROR if an undefined command is received.
 */

static ERROR_T devB_IfManageCommunications(void)
{
    ERROR_T eValue              = E_OK;
    UINT32_T uNotification      = 0x00;
    COMMANDS_LIST_T eCmd        = CMD_NONE;

    if (xTaskNotifyWait(0, UINT32_MAX, &uNotification, 0) == pdTRUE)
    {
        eCmd = (COMMANDS_LIST_T)uNotification;

        switch (eCmd)
        {
            case CMD_RESET:

                sDeviceB.bResetFlag = TRUE;

                log_Write("[DEVICE_B] reset received");

                break;

            default:

                eValue = E_ERROR;

                log_Write("[DEVICE_B] received undefined command");

                break;
        }
    }

    return eValue;
}

/**
 * @brief Callback invoked when the sleep timer expires.
 *
 * Behavior:
 * - Sets the flag indicating that the sleep timer has expired.
 * - Clears the flag indicating that the timer is running.
 *
 * The actual state transition (SLEEP → ACTIVE) is handled inside
 * devB_IfNextState(), keeping all state logic centralized.
 *
 * @param xTimer Handle of the expired timer (unused).
 */

static void devB_IfSleepTimerCallback(TIMER_T xTimer)
{
    sDeviceB.bSleepTimerExpired = TRUE;
    sDeviceB.bSleepTimerStarted = FALSE;
}
