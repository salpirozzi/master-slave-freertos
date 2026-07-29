/* includes */

#include "deviceA.h"
#include "deviceB.h"
#include "logging.h"
#include "common_typeDef.h"

/* definitions */
static TASK_T xTaskDeviceA;
static TASK_T xTaskDeviceB;

/* main function */

/**
 * @brief Entry point of the MASTER-SLAVE system.
 *
 * Responsibilities:
 * - Initialize logging, Device A, and Device B.
 * - Create FreeRTOS tasks for both devices.
 * - Start the FreeRTOS scheduler.
 *
 * If any initialization or task creation fails, the system logs an error
 * and does not start the scheduler.
 *
 * @return Program exit status (unused in FreeRTOS environment).
 */

int main(void)
{
    ERROR_T eValue = E_OK;

    eCheck(eValue, log_Init());

    eCheck(eValue, devB_Init());

    if (eValue == E_OK)
    {
        if (xTaskCreate(devB_Run, "devB", 1024, NULL, 2, &xTaskDeviceB) != pdPASS)
        {
            eValue = E_ERROR;

            log_Write("[MAIN] devB task creation failed");
        }
        else
        {
            log_Write("[MAIN] devB task created");
        }
    }
    
    eCheck(eValue, devA_Init(xTaskDeviceB));

    if (eValue == E_OK)
    {
        if (xTaskCreate(devA_Run, "devA", 512, NULL, 3, &xTaskDeviceA) != pdPASS)
        {
            eValue = E_ERROR;

            log_Write("[MAIN] devA task creation failed");
        }
        else
        {
            log_Write("[MAIN] devA task created");
        }
    }

    if (eValue == E_OK)
    {
        log_Write("[MAIN] starting scheduler");

        vTaskStartScheduler();
    }
    else
    {
        log_Write("[MAIN] error during startup");
    }

    return eValue;
}
