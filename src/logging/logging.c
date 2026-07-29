/* includes */

#include "logging.h"

/* declarations */

static SemaphoreHandle_t sLogMutex;

/* internal prototypes */

static void log_IfGetTimeString(CHAR_T *pvBuffer, SIZE_T sLen);

/* external functions */

ERROR_T log_Init(void)
{
    ERROR_T eValue = E_ERROR;

    sLogMutex = xSemaphoreCreateMutex();

    if (sLogMutex != NULL)
    {
        eValue = E_OK;
    }

    return eValue;
}

ERROR_T log_Write(const CHAR_T *pvFormat, ...)
{
    ERROR_T eValue = E_ERROR;

    ARGUMENTS_T lArgs;

    CHAR_T vTimeString[32];
    CHAR_T vLogString[256];

    memset(vTimeString, 0x00, sizeof(vTimeString));
    memset(vLogString, 0x00, sizeof(vLogString));

    /* ----------------------------------------------- */

    log_IfGetTimeString(vTimeString, sizeof(vTimeString));

    /* parsing arguments */

    va_start(lArgs, pvFormat);

    vsnprintf(vLogString, sizeof(vLogString), pvFormat, lArgs);

    va_end(lArgs);

    /* write to stdout */

    if (xSemaphoreTake(sLogMutex, portMAX_DELAY) == pdTRUE)
    {
        eValue = E_OK;
    }

    if (eValue == E_OK)
    {
        printf("[%s] %s\n", vTimeString, vLogString);

        fflush(stdout);

        xSemaphoreGive(sLogMutex);
    }

    return eValue;
}

/* internal functions */

/**
 * @brief Generate a human-readable timestamp string.
 *
 * Produces a timestamp in HH:MM:SS.mmm format using gettimeofday()
 * and localtime_r().
 *
 * @param pvBuffer Destination buffer.
 * @param sLen Length of the destination buffer.
 */

static void log_IfGetTimeString(CHAR_T *pvBuffer, SIZE_T sLen)
{
    TIMEVAL_T sTimeOfDay;
    TM_T sTimeInfo;

    gettimeofday(&sTimeOfDay, NULL);

    localtime_r(&sTimeOfDay.tv_sec, &sTimeInfo);

    snprintf(pvBuffer,
             sLen,
             "%02d:%02d:%02d.%03ld",
             sTimeInfo.tm_hour,
             sTimeInfo.tm_min,
             sTimeInfo.tm_sec,
             (sTimeOfDay.tv_usec / 1000));
}
