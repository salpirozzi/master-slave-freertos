#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Applicazione POSIX (Linux)
 *----------------------------------------------------------*/

/* Scheduler */
#define configUSE_PREEMPTION            1
#define configUSE_TIME_SLICING          1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

/* Tick */
#define configCPU_CLOCK_HZ              ( 1000000 )
#define configTICK_RATE_HZ              ( 1000 )
#define configTICK_TYPE_WIDTH_IN_BITS   TICK_TYPE_WIDTH_32_BITS


/* Priorità */
#define configMAX_PRIORITIES            10

/* Hook */
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0

/* Memoria */
#define configMINIMAL_STACK_SIZE        1024
#define configTOTAL_HEAP_SIZE           ( 1024 * 1024 )
#define configUSE_MALLOC_FAILED_HOOK    0

/* Timer task */
#define configUSE_TIMERS                1
#define configTIMER_TASK_PRIORITY       2
#define configTIMER_QUEUE_LENGTH        10
#define configTIMER_TASK_STACK_DEPTH    2048

/* Mutex, semafori, ecc */
#define configUSE_MUTEXES               1
#define configUSE_COUNTING_SEMAPHORES   1
#define configUSE_RECURSIVE_MUTEXES     1

/* Queue */
#define configQUEUE_REGISTRY_SIZE       10

/* POSIX errno */
#define configUSE_POSIX_ERRNO           1

/* Debug */
#define configCHECK_FOR_STACK_OVERFLOW  0

/* Assert */
#define configASSERT(x) if((x)==0) { printf("ASSERT FAILED\n"); abort(); }

/* Includes */
#define INCLUDE_vTaskDelay              1

#endif /* FREERTOS_CONFIG_H */
