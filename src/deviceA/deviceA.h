#ifndef _DEVICEA_H
#define _DEVICEA_H

/* includes */

#include <stdio.h>
#include <stdlib.h>

#include "deviceB.h"
#include "logging.h"
#include "common_typeDef.h"

/* definitions */

#define DEVA_MAX_FAULT          (3)     /* Max consecutive faults before RESET */
#define DEVA_POLLING_PERIOD     (500)   /* Polling period in ms */

/**
 * @brief Internal instance structure for Device A.
 *
 * Fields:
 * - eCurrentState:
 *      Current state of Device A.
 *
 * - uFaultCounter:
 *      Counts consecutive FAULT states from Device B.
 *
 * - eDevBState:
 *      Cached state of Device B, updated periodically.
 *
 * - xTaskDeviceB:
 *      Task handle of Device B, required to send asynchronous RESET commands.
 */

typedef struct DEVICE_A_INSTANCE
{
    DEVICE_A_STATES_T   eCurrentState;
    UINT8_T             uFaultCounter;
    DEVICE_B_STATES_T   eDevBState;
    TASK_T              xTaskDeviceB;
} DEVICE_A_INSTANCE;

/* functions prototypes */

/**
 * @brief Main task function for Device A.
 *
 * Implements the supervisory logic:
 * - Retrieves Device B's state using devB_GetState().
 * - Updates the cached state when valid.
 * - Processes the state through Device A's internal state machine.
 * - Sleeps for DEVA_POLLING_PERIOD milliseconds.
 *
 * Runs indefinitely.
 *
 * @param pvArgs Unused task argument.
 */

void devA_Run(void *pvArgs);

/**
 * @brief Initializes Device A.
 *
 * Behavior:
 * - Validates the task handle of Device B.
 * - Initializes internal fields (state, counters, cached B state).
 * - Stores the task handle of Device B for RESET signaling.
 *
 * Logs initialization status.
 *
 * @param xTaskDeviceB Task handle of Device B.
 *
 * @return E_OK if initialization succeeded, E_ERROR otherwise.
 */

ERROR_T devA_Init(const TASK_T xTaskDeviceB);

#endif /* endif _DEVICEA_H */
