/******************************************************************************

@file  app_peripheral.c

@brief This example file demonstrates how to activate the peripheral role with
the help of BLEAppUtil APIs.

Two structures are used for event handling, one for connection events and one
for advertising events.
In each, eventMask is used to specify the events that will be received
and handled.
In addition, fill the BLEAppUtil_AdvInit_t structure with variables generated
by the Sysconfig.

In the events handler functions, write what actions are done after each event.
In this example, after a connection is made, activation is performed for
re-advertising up to the maximum connections.

In the Peripheral_start() function at the bottom of the file, registration,
initialization and activation are done using the BLEAppUtil API functions,
using the structures defined in the file.

More details on the functions and structures can be seen next to the usage.

Group: WCS, BTS
Target Device: cc13xx_cc26xx

******************************************************************************

 Copyright (c) 2022-2023, Texas Instruments Incorporated
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

#if defined( HOST_CONFIG ) && ( HOST_CONFIG & ( PERIPHERAL_CFG ) )

//*****************************************************************************
//! Includes
//*****************************************************************************
#include "ti_ble_config.h"
#include <ti/bleapp/ble_app_util/inc/bleapputil_api.h>

//*****************************************************************************
//! Prototypes
//*****************************************************************************
void Peripheral_AdvEventHandler(uint32 event, BLEAppUtil_msgHdr_t *pMsgData);
void Peripheral_GAPConnEventHandler(uint32 event, BLEAppUtil_msgHdr_t *pMsgData);

//*****************************************************************************
//! Globals
//*****************************************************************************

BLEAppUtil_EventHandler_t peripheralConnHandler =
{
    .handlerType    = BLEAPPUTIL_GAP_CONN_TYPE,
    .pEventHandler  = Peripheral_GAPConnEventHandler,
    .eventMask      = BLEAPPUTIL_LINK_ESTABLISHED_EVENT |
                      BLEAPPUTIL_LINK_PARAM_UPDATE_REQ_EVENT |
                      BLEAPPUTIL_LINK_TERMINATED_EVENT,
};

BLEAppUtil_EventHandler_t peripheralAdvHandler =
{
    .handlerType    = BLEAPPUTIL_GAP_ADV_TYPE,
    .pEventHandler  = Peripheral_AdvEventHandler,
    .eventMask      = BLEAPPUTIL_ADV_START_AFTER_ENABLE |
                      BLEAPPUTIL_ADV_END_AFTER_DISABLE
};

//! Store handle needed for each advertise set
uint8 peripheralAdvHandle_1;

//! Advertise param, needed for each advertise set, Generate by Sysconfig
const BLEAppUtil_AdvInit_t advSetInitParamsSet_1 =
{
    /* Advertise data and length */
    .advDataLen        = sizeof(advData1),
    .advData           = advData1,

    /* Scan respond data and length */
    .scanRespDataLen   = sizeof(scanResData1),
    .scanRespData      = scanResData1,

    .advParam          = &advParams1
};

const BLEAppUtil_AdvStart_t advSetStartParamsSet_1 =
{
    /* Use the maximum possible value. This is the spec-defined maximum for */
    /* directed advertising and infinite advertising for all other types */
    .enableOptions         = GAP_ADV_ENABLE_OPTIONS_USE_MAX,
    .durationOrMaxEvents   = 0
};

//*****************************************************************************
//! Functions
//*****************************************************************************

/*********************************************************************
 * @fn      Peripheral_AdvEventHandler
 *
 * @brief   The purpose of this function is to handle advertise events
 *          that rise from the GAP and were registered in
 *          @ref BLEAppUtil_RegisterGAPEvent
 *
 * @param   event - message event.
 * @param   pMsgData - pointer to message data.
 *
 * @return  none
 */
void Peripheral_AdvEventHandler(uint32 event, BLEAppUtil_msgHdr_t *pMsgData)
{
    BLEAppUtil_AdvEventData_t *advData = (BLEAppUtil_AdvEventData_t *)pMsgData;

    switch(event)
    {
        case BLEAPPUTIL_ADV_START_AFTER_ENABLE:
        {
            DebugP_log1("ADV_START_AFTER_ENABLE: Peripheral role, advhandle: %d", advData->pBuf->advHandle);
            break;
        }

        case BLEAPPUTIL_ADV_END_AFTER_DISABLE:
        {
            DebugP_log1("ADV_START_AFTER_DISABLE: Peripheral role, advhandle: %d", advData->pBuf->advHandle);

            break;
        }

        default:
        {
            break;
        }
    }
}

/*********************************************************************
 * @fn      Peripheral_GAPConnEventHandler
 *
 * @brief   The purpose of this function is to handle connection related
 *          events that rise from the GAP and were registered in
 *          @ref BLEAppUtil_RegisterGAPEvent
 *
 * @param   event - message event.
 * @param   pMsgData - pointer to message data.
 *
 * @return  none
 */
void Peripheral_GAPConnEventHandler(uint32 event, BLEAppUtil_msgHdr_t *pMsgData)
{
    switch(event)
    {
        case BLEAPPUTIL_LINK_ESTABLISHED_EVENT:
        {
            gapEstLinkReqEvent_t *gapEstMsg = (gapEstLinkReqEvent_t *)pMsgData;

            if(gapEstMsg->connRole == BLEAPPUTIL_PERIPHERAL_ROLE)
            {
                /*! Print the peer address and connection handle number */
                DebugP_log2("LINK_ESTABLISHED_EVENT: Peripheral role "
                "Connected to %s, connectionHandle = %d",
                (uintptr_t)BLEAppUtil_convertBdAddr2Str(gapEstMsg->devAddr),
                gapEstMsg->connectionHandle);

                /*! Print the number of current connections */
                DebugP_log1("LINK_ESTABLISHED_EVENT: Peripheral role "
                            "Num Conns = %d", linkDB_NumActive());

                if(linkDB_NumActive() < linkDB_NumConns())
                {
                    BLEAppUtil_advStart(peripheralAdvHandle_1, &advSetStartParamsSet_1);
                }
                else
                {
                    BLEAppUtil_advStop(peripheralAdvHandle_1);
                }
            }
            break;
        }

        case BLEAPPUTIL_LINK_TERMINATED_EVENT:
        {
            gapTerminateLinkEvent_t *gapTermMsg = (gapTerminateLinkEvent_t *)pMsgData;

            /*! Print the connHandle and termination reason */
            DebugP_log2("LINK_TERMINATED_EVENT: Peripheral role, "
                        "connectionHandle = %d, reason = %d",
                        gapTermMsg->connectionHandle, gapTermMsg->reason);

            /*! Print the number of current connections */
            DebugP_log1("LINK_TERMINATED_EVENT: Peripheral role "
                        "Num Conns = %d", linkDB_NumActive());

            BLEAppUtil_advStart(peripheralAdvHandle_1, &advSetStartParamsSet_1);
            break;
        }
        case BLEAPPUTIL_LINK_PARAM_UPDATE_REQ_EVENT:
        {
            gapUpdateLinkParamReqEvent_t *pReq = (gapUpdateLinkParamReqEvent_t *)pMsgData;

            // Only accept connection intervals with slave latency of 0
            // This is just an example of how the application can send a response
            if(pReq->req.connLatency == 0)
            {
                BLEAppUtil_paramUpdateRsp(pReq,TRUE);
            }
            else
            {
                BLEAppUtil_paramUpdateRsp(pReq,FALSE);
            }

            break;
        }

        default:
        {
            break;
        }
    }
}

/*********************************************************************
 * @fn      Peripheral_start
 *
 * @brief   This function is called after stack initialization,
 *          the purpose of this function is to initialize and
 *          register the specific events handlers of the peripheral
 *          application module
 *
 * @return  SUCCESS, errorInfo
 */
bStatus_t Peripheral_start()
{
    bStatus_t status;

    DebugP_log0("Peripheral_start: Register Handlers");

    status = BLEAppUtil_registerEventHandler(&peripheralConnHandler);
    status = BLEAppUtil_registerEventHandler(&peripheralAdvHandler);
    if(status != SUCCESS)
    {
        return(status);
    }

    DebugP_log0("Peripheral_start: Init Adv Set 1");

    status = BLEAppUtil_initAdvSet(&peripheralAdvHandle_1, &advSetInitParamsSet_1);
    if(status != SUCCESS)
    {
        return(status);
    }

    DebugP_log0("Peripheral_start: Start Adv Set 1");

    status = BLEAppUtil_advStart(peripheralAdvHandle_1, &advSetStartParamsSet_1);
    if(status != SUCCESS)
    {
        return(status);
    }

    return SUCCESS;
}

#endif // ( HOST_CONFIG & ( PERIPHERAL_CFG ) )
