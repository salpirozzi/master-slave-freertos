#ifndef _DEVICEB_H
#define _DEVICEB_H

/* includes */

#include <stdio.h>
#include <stdlib.h>

#include "logging.h"
#include "common_typeDef.h"

/* definitions */

#define DEVB_TASK_PERIOD        (1000)      /* Task period in ms */
#define DEVB_FAULT_TIME_MS      (10000)     /* Time before entering FAULT */
#define DEVB_SLEEP_TIME_MS      (20000)     /* Time before returning to SLEEP */

#define DEVB_FAULT_TICKS        (DEVB_FAULT_TIME_MS / DEVB_TASK_PERIOD)
#define DEVB_SLEEP_TICKS        (DEVB_SLEEP_TIME_MS / DEVB_TASK_PERIOD)

/**
 * @brief Internal instance structure for Device B.
 *
 * Fields:
 * - eCurrentState:
 *      Current state of Device B.
 *
 * - uTickCounter:
 *      Internal tick counter incremented every DEVB_TASK_PERIOD ms.
 *      Used for timing-based transitions (FAULT, SLEEP).
 *
 * - bResetFlag:
 *      Set when Device A sends CMD_RESET.
 *      Causes FAULT → SLEEP transition on next state update.
 *
 * - xStateDevB:
 *      Mutex protecting access to the internal state.
 *
 * - xSleepTimer:
 *      FreeRTOS one-shot timer used to detect prolonged SLEEP state.
 *      When expired, Device B transitions from SLEEP → ACTIVE.
 *
 * - bSleepTimerExpired:
 *      Set by the timer callback when the sleep timer expires.
 *
 * - bSleepTimerStarted:
 *      Indicates whether the sleep timer is currently running.
 */

typedef struct DEVICE_B_INSTANCE
{
    DEVICE_B_STATES_T   eCurrentState;
    UINT8_T             uTickCounter;
    BOOL_T              bResetFlag;
    SEMAPHORE_T         xStateDevB;
    TIMER_T             xSleepTimer;
    BOOL_T              bSleepTimerExpired;
    BOOL_T              bSleepTimerStarted;
} DEVICE_B_INSTANCE;

/* functions prototypes */

/**
 * @brief Main task function for Device B.
 *
 * Implements the autonomous behavior of Device B:
 *
 * - Attempts to acquire the state mutex (non-blocking).
 * - Computes the next state using devB_IfNextState().
 * - Releases the mutex.
 * - Processes asynchronous commands from Device A using devB_IfManageCommunications().
 * - Sleeps for DEVB_TASK_PERIOD milliseconds.
 *
 * The task runs indefinitely.
 *
 * @param pvArgs Unused task argument.
 */

void devB_Run(void *pvArgs);

/**
 * @brief Initializes Device B.
 *
 * Creates the mutex protecting the internal state.
 * Initializes all internal fields:
 * - Tick counter set to zero.
 * - Current state set to DEVB_SLEEP.
 * - Reset flag cleared.
 * - Sleep timer created (but not started).
 *
 * Logs initialization status.
 *
 * @return E_OK if initialization succeeded, E_ERROR otherwise.
 */

ERROR_T devB_Init();

/**
 * @brief Thread-safe retrieval of Device B's current state.
 *
 * Attempts to acquire the internal state mutex (non-blocking).
 * If successful, writes the current state into the caller-provided pointer.
 * If the mutex is unavailable, returns DEVB_UNDEFINED.
 *
 * @param peDevBState Pointer to store the retrieved state.
 *
 * @return E_OK if the pointer is valid, E_ERROR otherwise.
 */

ERROR_T devB_GetState(DEVICE_B_STATES_T *peDevBState);

#endif /* endif _DEVICEB_H */
