/******************************************************************************

@file  QueueP_tirtos.c

@brief This file contains TI-RTOS OS abstraction layer

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2023, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <stdlib.h>
#include <QueueP.h>
#include <ti/sysbios/knl/Queue.h>

// RTOS queue for profile/app messages.
typedef struct _queueRec_
{
  Queue_Elem _elem;          // queue element
  uint8_t *pData;            // pointer to app data
} queueRec_t;

/*********************************************************************
 * @fn      QueueP_create - OS abstraction Api
 *
 * @brief   Create queue handler.
 */
QueueP_Handle QueueP_create()
{
  Queue_Struct *pQueueStruct = (Queue_Struct *)malloc(sizeof(Queue_Struct));

  // Construct a Queue instance.
  Queue_construct(pQueueStruct, NULL);

  return Queue_handle(pQueueStruct);
}

/*********************************************************************
 * @fn      QueueP_enqueue - OS abstraction Api
 *
 * @brief   enqueue message.
 */
uint8_t QueueP_enqueue(QueueP_Handle msgQueue, uint8_t *pMsg)
{
  queueRec_t *pRec;

  // Allocated space for queue node.
  if ((pRec = (queueRec_t *)malloc(sizeof(queueRec_t))))
  {
    pRec->pData = pMsg;

    // This is an atomic operation
    Queue_put(msgQueue, &pRec->_elem);

    return TRUE;
  }

  return FALSE;
}

/*********************************************************************
 * @fn      QueueP_dequeue - OS abstraction Api
 *
 * @brief   dequeue message.
 */
uint8_t *QueueP_dequeue(QueueP_Handle msgQueue)
{
  queueRec_t *pRec = Queue_get(msgQueue);

  if (pRec != (queueRec_t *)msgQueue)
  {
    uint8_t *pData = pRec->pData;

    // Free the queue node
    // Note:  this does not free space allocated by data within the node.
    free(pRec);

    return pData;
  }

  return NULL;
}

/*********************************************************************
 * @fn      QueueP_empty - OS abstraction Api
 *
 * @brief   check if queue empty.
 */
uint8_t QueueP_empty(QueueP_Handle queueHandle)
{
  return Queue_empty((Queue_Handle)queueHandle);
}
