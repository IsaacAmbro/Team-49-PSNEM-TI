/******************************************************************************

@file  EventP_tirtos.c

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
#include <EventP.h>
#include <ti/sysbios/knl/Event.h>

/*********************************************************************
 * @fn      EventP_create - OS abstraction Api
 *
 * @brief   Create event handler.
 */
EventP_Handle EventP_create()
{
    return (EventP_Handle)Event_create(NULL, NULL);
}

/*********************************************************************
 * @fn      EventP_pend - OS abstraction Api
 *
 * @brief   wait on event.
 */
uint32_t EventP_pend(EventP_Handle eventHandle, uint32_t eventMask, bool waitForAll, uint32_t timeout)
{
    return Event_pend((Event_Handle)eventHandle, 0, eventMask, timeout);
}

/*********************************************************************
 * @fn      EventP_post - OS abstraction Api
 *
 * @brief   post event.
 */
void EventP_post(EventP_Handle eventHandle, uint32_t events)
{
    Event_post((Event_Handle)eventHandle, events);
}
