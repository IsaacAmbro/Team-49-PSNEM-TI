/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       TaskP.h
 *
 *  @brief      Task module for the RTOS Porting Interface
 *
 *  TaskP objects are RTOS threads backed by OS-specific thread or task objects.
 *  Task functions will run according to the rules of the underlying scheduler,
 *  with higher priority tasks executing first.
 *
 *  Tasks require a stack and a control struct to operate, which can either be
 *  allocated statically with TaskP_construct or dynamically with TaskP_create.
 *  The stack should be large enough to contain at least your deepest call stack
 *  plus an interrupt frame.
 *
 *  Task Functions:
 *  The void* argument will be NULL by default, but you can set a value using
 *  TaskP_Params. Task functions should never return, as the behaviour after a
 *  task has returned is implementation-dependent and TaskP does not provide a
 *  mechanism for OS-independent task deletion. See your OS documentation for
 *  details.
 *  ============================================================================
 */

#ifndef ti_dpl_TaskP__include
#define ti_dpl_TaskP__include

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief    Number of bytes greater than or equal to the size of any RTOS Task object.
 *
 *  BIOS 6.x: 80
 *  BIOS 7.x: 80
 *  FreeRTOS: 88
 */
#define TaskP_STRUCT_SIZE   (88)

/*!
 *  @brief    TaskP structure.
 *
 *  Opaque structure that should be large enough to hold any of the RTOS specific TaskP objects.
 */
typedef union TaskP_Struct {
    uint32_t dummy;  /*!< Align object */
    uint8_t  data[TaskP_STRUCT_SIZE];
} TaskP_Struct;

/*!
 *  @brief    Enum returned from TaskP_getState().
 */
typedef enum {
    /*! This task is actively running */
    TaskP_State_RUNNING,
    /*! The task is ready to run, but not currently running */
    TaskP_State_READY,
    /*! The task is blocked */
    TaskP_State_BLOCKED,
    /*! The task has been deleted */
    TaskP_State_DELETED,
    /*! The task is not found, inactive, or in an otherwise invalid state */
    TaskP_State_INVALID
} TaskP_State;

/*!
 *  @brief Opaque client reference to an instance of a TaskP
 *
 *  A TaskP_Handle returned from create or construct represents that instance.
 */
typedef TaskP_Struct* TaskP_Handle;

typedef struct {
    /*! Task function argument. Default: NULL */
    void* arg;
    /*! Task priority. Higher values represent higher priorities. */
    int priority;
    /*! Task stack size. */
    size_t stackSize;
    /*! @brief Task stack pointer. NULL should be used when calling TaskP_create.
     *  A pointer to a current stack should be passed for TaskP_construct.
     */
    void* stack;
} TaskP_Params;

/*!
 *  @brief Task function definition, passed to create and construct
 *
 *  This function should never return.
 */
typedef void (*TaskP_Function)(void*);

/*!
 * @brief Create a TaskP, allocating memory on the heap.
 *
 * Creates a new TaskP and registers it with the OS scheduler. The task object
 * and the entire stack will be allocated on the heap - make sure you have a
 * sufficiently large heap. Stack allocation size is controlled by params.
 *
 * If the program is already executing a task and the new task has a higher
 * priority the new task will be scheduled immediately; in this case
 * TaskP_create() will not return until the new task blocks. To avoid this (for
 * example when creating interdependent tasks at once) use
 * TaskP_disableScheduler() and TaskP_restoreScheduler() to prevent the context
 * switch.
 *
 * This API cannot be called from interrupt contexts.
 *
 * @retval TaskP handle (NULL on failure)
 */
extern TaskP_Handle TaskP_create(TaskP_Function fxn, const TaskP_Params *params);

/*!
 * @brief Construct a TaskP from statically allocated memory.
 *
 * TaskP_construct creates a new task object. TaskP_construct returns the handle
 * of the new task object or NULL if the task could not be created.
 *
 * To use TaskP_construct you must set both point @c params.stack to a valid
 * preallocated memory location of at least @c params.stackSize.
 *
 * This API cannot be called from interrupt contexts. For FreeRTOS,
 * configSUPPORT_STATIC_ALLOCATION has to be set to 1 in FreeRTOSConfig.h. See
 * 'Configuration with FreeRTOS' in the Core SDK User's Guide for how to do this.
 *
 * @retval TaskP handle (NULL on failure)
 */
extern TaskP_Handle TaskP_construct(TaskP_Struct *obj, TaskP_Function fxn, const TaskP_Params *params);

/*!
 * @brief Get the current state of a task handle.
 *
 * Returns the state of the referenced task at the time this function was
 * called. The return value is not guaranteed to match the state of the task
 * when the calling function tests the return value. For example, the referenced
 * task might have unblocked as a result of an interrupt, but the value may
 * still read TaskP_State_BLOCKED.
 *
 * For FreeRTOS, INCLUDE_eTaskGetState has to be set to 1 in FreeRTOSConfig.h.
 * See 'Configuration with FreeRTOS' in the Core SDK User's Guide for how to do this.
 *
 * @retval Current state of the task pointed to by the task parameter
 */
extern TaskP_State TaskP_getState(TaskP_Handle task);

/*!
 * @brief Get the currently executing task handle.
 *
 * Must be called from task context.
 *
 * @retval The handle for the calling task
 */
extern TaskP_Handle TaskP_getCurrentTask(void);

/*!
 * @brief  Function to disable task scheduling
 *
 * This function can be called multiple times, but must unwound in the reverse
 * order. For example
 * @code
 * uintptr_t key1, key2;
 * key1 = TaskP_disableScheduler();
 * key2 = TaskP_disableScheduler();
 * TaskP_restoreScheduler(key2);
 * TaskP_restoreScheduler(key1);
 * @endcode
 *
 * This API cannot be called from interrupt contexts.
 *
 * @return A key to pass to TaskP_restoreScheduler to re-enable the scheduler.
 */
extern uintptr_t TaskP_disableScheduler(void);

/*!
 * @brief  Function to re-enable task scheduling
 *
 * This API cannot be called from interrupt contexts.
 *
 * @param  key returned from TaskP_disableScheduler
 */
extern void TaskP_restoreScheduler(uintptr_t key);

/*!
 * @brief Create a scheduler point, yielding to equal priority tasks.
 *
 * Task_yield yields the processor to another task of equal priority. A task
 * switch occurs when you call Task_yield if there is an equal priority task
 * ready to run.
 */
extern void TaskP_yield(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_dpl_TaskP__include */
