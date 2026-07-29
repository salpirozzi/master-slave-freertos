#ifndef _LOGGING_H
#define _LOGGING_H

/* includes */

#include <time.h>
#include <sys/time.h>
#include <stdarg.h>

#include "common_typeDef.h"

/* functions prototypes */

/**
 * @brief Initialize the logging module.
 *
 * Creates a FreeRTOS mutex used to serialize access to the logging output.
 * Ensures that concurrent tasks do not interleave log messages.
 *
 * @return E_OK if the mutex was successfully created, E_ERROR otherwise.
 */

ERROR_T log_Init(void);

/**
 * @brief Write a formatted log message to stdout in a thread-safe manner.
 *
 * Steps:
 * 1. Generate timestamp string.
 * 2. Format the message using variadic arguments.
 * 3. Acquire the logging mutex.
 * 4. Print the timestamped message.
 * 5. Flush stdout.
 * 6. Release the mutex.
 *
 * @param pvFormat Format string (printf-style).
 * @param ... Additional formatting arguments.
 *
 * @return E_OK if the message was printed successfully, E_ERROR otherwise.
 */

ERROR_T log_Write(const CHAR_T *pvFormat, ...);

#endif /* endif _LOGGING_H */