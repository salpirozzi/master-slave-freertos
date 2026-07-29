/* includes */

#include "deviceA.h"

/* declarations */

static DEVICE_A_INSTANCE sDeviceA;

/* internal prototypes */

static void devA_IfDevBHandler(const DEVICE_B_STATES_T eDevBState);

/* external functions */

void devA_Run(void *pvArgs)
{
    ERROR_T eValue                  = E_ERROR;
    DEVICE_B_STATES_T eDevBStateTmp = DEVB_UNDEFINED;

    for (;;)
    {
        eValue = devB_GetState(&eDevBStateTmp);

        if ((eValue == E_OK) && (eDevBStateTmp != DEVB_UNDEFINED))
        {
            sDeviceA.eDevBState = eDevBStateTmp;
        }

        devA_IfDevBHandler(sDeviceA.eDevBState);

        vTaskDelay(pdMS_TO_TICKS(DEVA_POLLING_PERIOD));
    }
}

ERROR_T devA_Init(const TASK_T xTaskDeviceB)
{
    ERROR_T eValue = E_ERROR;

    if (xTaskDeviceB != NULL)
    {
        eValue = E_OK;
    }

    if (eValue == E_OK)
    {
        sDeviceA.eCurrentState  = DEVA_IDLE;
        sDeviceA.eDevBState     = DEVB_UNDEFINED;
        sDeviceA.uFaultCounter  = 0;
        sDeviceA.xTaskDeviceB   = xTaskDeviceB;

        log_Write("[DEVICE_A] initialized");
    }
    else
    {
        log_Write("[DEVICE_A] not initialized");
    }

    return E_OK;
}

/* internal functions */

/**
 * @brief Device A state machine handler.
 *
 * Processes Device B's state and updates Device A accordingly.
 *
 * State transitions:
 *
 * - DEVA_IDLE:
 *      → DEVA_PROCESSING when Device B becomes ACTIVE.
 *
 * - DEVA_PROCESSING:
 *      → DEVA_ERROR when Device B enters FAULT.
 *      → DEVA_IDLE when Device B returns to SLEEP.
 *
 * - DEVA_ERROR:
 *      → If Device B remains in FAULT:
 *          • Increment fault counter.
 *          • When counter reaches DEVA_MAX_FAULT:
 *              - Send CMD_RESET to Device B via xTaskNotify().
 *              - Transition to DEVA_IDLE.
 *              - Reset fault counter.
 *
 *      → If Device B recovers (not FAULT):
 *          • Transition to DEVA_IDLE.
 *          • Reset fault counter.
 *
 * All transitions are logged.
 *
 * @param eDevBState Current state of Device B.
 */

static void devA_IfDevBHandler(const DEVICE_B_STATES_T eDevBState)
{
    CHAR_T cTransition[128];
    DEVICE_A_STATES_T ePreviousState = DEVA_UNDEFINED;

    memset(cTransition, 0x00, sizeof(cTransition));

    /* ----------------------------------------------- */

    ePreviousState = sDeviceA.eCurrentState;

    switch (sDeviceA.eCurrentState)
    {
        case DEVA_IDLE:

            if (eDevBState == DEVB_ACTIVE)
            {
                sDeviceA.eCurrentState = DEVA_PROCESSING;
            }

            break;

        case DEVA_PROCESSING:

            if (eDevBState == DEVB_FAULT)
            {
                sDeviceA.eCurrentState = DEVA_ERROR;
                sDeviceA.uFaultCounter = 1;
            }
            else if (eDevBState == DEVB_SLEEP)
            {
                sDeviceA.eCurrentState = DEVA_IDLE;
            }

            break;

        case DEVA_ERROR:

            if (eDevBState == DEVB_FAULT)
            {
                sDeviceA.uFaultCounter++;

                if (sDeviceA.uFaultCounter >= DEVA_MAX_FAULT)
                {
                    sDeviceA.eCurrentState  = DEVA_IDLE;
                    sDeviceA.uFaultCounter  = 0;

                    if (xTaskNotify(sDeviceA.xTaskDeviceB,
                                    CMD_RESET,
                                    eSetValueWithOverwrite) == pdTRUE)
                    {
                        log_Write("[DEVICE_A] reset requested");
                    }
                    else
                    {
                        log_Write("[DEVICE_A] failed to request reset");
                    }
                }
            }
            else
            {
                sDeviceA.eCurrentState = DEVA_IDLE;
                sDeviceA.uFaultCounter = 0;
            }

            break;

        default:
            break;
    }

    if (ePreviousState != sDeviceA.eCurrentState)
    {
        log_Write("[DEVICE_A] transition %s => %s",
                  DEVICE_A_STATE_STR[ePreviousState],
                  DEVICE_A_STATE_STR[sDeviceA.eCurrentState]);
    }
}
