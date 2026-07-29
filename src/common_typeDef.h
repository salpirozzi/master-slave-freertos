#ifndef _COMMON_TYPEDEF_H
#define _COMMON_TYPEDEF_H

/* std includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* free rtos */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

/* semantic redefinitions */

typedef SemaphoreHandle_t   SEMAPHORE_T;
typedef TaskHandle_t        TASK_T;
typedef TimerHandle_t       TIMER_T;
typedef uint8_t             UINT8_T;
typedef uint32_t            UINT32_T;
typedef char                CHAR_T;
typedef size_t              SIZE_T;
typedef va_list             ARGUMENTS_T;
typedef struct timeval      TIMEVAL_T;
typedef struct tm           TM_T;

/* definitions */

#define eCheck(eValue, pFunction)   \
   if ((eValue) == E_OK)            \
   {                                \
      (eValue) = (pFunction);       \
   }

typedef enum ERROR_TE
{
    E_OK    = 0,
    E_ERROR = 1
} ERROR_T;

typedef enum BOOL_TE
{
    FALSE   = 0,
    TRUE    = 1
} BOOL_T;

typedef enum DEVICE_A_STATES_TE
{
    DEVA_IDLE        = 0,
    DEVA_PROCESSING  = 1,
    DEVA_ERROR       = 2,
    DEVA_UNDEFINED   = 3
} DEVICE_A_STATES_T;

typedef enum DEVICE_B_STATES_TE
{
    DEVB_SLEEP      = 0,
    DEVB_ACTIVE     = 1,
    DEVB_FAULT      = 2,
    DEVB_UNDEFINED  = 3
} DEVICE_B_STATES_T;

typedef enum COMMANDS_LIST_TE
{
    CMD_NONE        = 0,
    CMD_RESET       = 1
} COMMANDS_LIST_T;

static const char *DEVICE_A_STATE_STR[] =
{
    "IDLE",
    "PROCESSING",
    "ERROR",
    "UNDEFINED"
};

static const char *DEVICE_B_STATE_STR[] =
{
    "SLEEP",
    "ACTIVE",
    "FAULT",
    "UNDEFINED"
};

static const char *COMMAND_STR[] =
{
    "NONE",
    "RESET"
};

#endif /* endif _COMMON_TYPEDEF_H */