

#include <bcomdef.h>
#include <gapgattserver.h>
#include <gap_advertiser.h>
#include <gapbondmgr.h>
#include "ti_ble_config.h"


// The GAP profile role
uint8_t profileRole = GAP_PROFILE_PERIPHERAL | GAP_PROFILE_CENTRAL;
// GAP GATT Service (GGS) parameters
uint8_t attDeviceName[GAP_DEVICE_NAME_LEN]= "Basic BLE project";

uint8_t * pRandomAddress = NULL;

/*********************************************************************
 * Bond Manager Configuration
 */

gapBondParams_t gapBondParams = {
    .pairMode             = GAPBOND_PAIRING_MODE_INITIATE,
    .mitm                 = true,
    .ioCap                = GAPBOND_IO_CAP_DISPLAY_ONLY,
    .bonding              = true,
    .secureConnection     = GAPBOND_SECURE_CONNECTION_ALLOW,
    .authenPairingOnly    = false,
    .autoSyncWL           = false,
    .eccReGenPolicy       = 0,
    .KeySize              = 16,
    .removeLRUBond        = false,
    .KeyDistList          = GAPBOND_KEYDIST_MENCKEY | GAPBOND_KEYDIST_MIDKEY | GAPBOND_KEYDIST_MSIGN | GAPBOND_KEYDIST_SENCKEY | GAPBOND_KEYDIST_SIDKEY | GAPBOND_KEYDIST_SSIGN,
    .eccDebugKeys         = false,
    .eraseBondWhileInConn = false,
    .bondFailAction       = GAPBOND_FAIL_TERMINATE_LINK,
    .sameIrkAction        = GAPBOND_SAME_IRK_UPDATE_BOND_REC
};

uint8_t pairMode                =    GAPBOND_PAIRING_MODE_INITIATE;
uint8_t mitm                    =    true;
uint8_t ioCap                   =    GAPBOND_IO_CAP_DISPLAY_ONLY;
uint8_t bonding                 =    true;
uint8_t secureConnection        =    GAPBOND_SECURE_CONNECTION_ALLOW;
uint8_t authenPairingOnly       =    false;
uint8_t autoSyncWL              =    false;
uint8_t eccReGenPolicy          =    0;
uint8_t KeySize                 =    16;
uint8_t removeLRUBond           =    false;
uint8_t bondFailAction          =    GAPBOND_FAIL_TERMINATE_LINK;
uint8_t KeyDistList             =    GAPBOND_KEYDIST_MENCKEY | GAPBOND_KEYDIST_MIDKEY | GAPBOND_KEYDIST_MSIGN | GAPBOND_KEYDIST_SENCKEY | GAPBOND_KEYDIST_SIDKEY | GAPBOND_KEYDIST_SSIGN;
uint8_t eccDebugKeys            =    false;
uint8_t allowDebugKeys          =    true;
uint8_t eraseBondWhileInConn    =    false;
uint8_t sameIrkAction           =    GAPBOND_SAME_IRK_UPDATE_BOND_REC;

void setBondManagerParameters()
{
    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
    GAPBondMgr_SetParameter(GAPBOND_SECURE_CONNECTION, sizeof(uint8_t), &secureConnection);
    // Set Authenticated Pairing Only mode
    GAPBondMgr_SetParameter(GAPBOND_AUTHEN_PAIRING_ONLY, sizeof(uint8_t), &authenPairingOnly);
    // Set Auto Whitelist Sync
    GAPBondMgr_SetParameter(GAPBOND_AUTO_SYNC_WL, sizeof(uint8_t), &autoSyncWL);
    GAPBondMgr_SetParameter(GAPBOND_ECCKEY_REGEN_POLICY, sizeof(uint8_t), &eccReGenPolicy);
    GAPBondMgr_SetParameter(GAPBOND_KEYSIZE, sizeof(uint8_t), &KeySize);
    GAPBondMgr_SetParameter(GAPBOND_LRU_BOND_REPLACEMENT, sizeof(uint8_t), &removeLRUBond);
    GAPBondMgr_SetParameter(GAPBOND_BOND_FAIL_ACTION, sizeof(uint8_t), &bondFailAction);
    GAPBondMgr_SetParameter(GAPBOND_KEY_DIST_LIST, sizeof(uint8_t), &KeyDistList);
    // Set Secure Connection Debug Keys
    GAPBondMgr_SetParameter(GAPBOND_SC_HOST_DEBUG, sizeof(uint8_t), &eccDebugKeys);
    // Set Allow Debug Keys
    GAPBondMgr_SetParameter(GAPBOND_ALLOW_DEBUG_KEYS, sizeof(uint8_t), &allowDebugKeys);
    // Set the Erase bond While in Active Connection Flag
    GAPBondMgr_SetParameter(GAPBOND_ERASE_BOND_IN_CONN, sizeof(uint8_t), &eraseBondWhileInConn);
    GAPBondMgr_SetParameter(GAPBOND_SAME_IRK_OPTION, sizeof(uint8_t), &sameIrkAction);
}


GapAdv_params_t advParams1 = {
  .eventProps =   GAP_ADV_PROP_CONNECTABLE | GAP_ADV_PROP_LEGACY | GAP_ADV_PROP_SCANNABLE,
  .primIntMin =   160,
  .primIntMax =   160,
  .primChanMap =  GAP_ADV_CHAN_ALL,
  .peerAddrType = PEER_ADDRTYPE_PUBLIC_OR_PUBLIC_ID,
  .peerAddr =     { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa },
  .filterPolicy = GAP_ADV_WL_POLICY_ANY_REQ,
  .txPower =      GAP_ADV_TX_POWER_NO_PREFERENCE,
  .primPhy =      GAP_ADV_PRIM_PHY_1_MBPS,
  .secPhy =       GAP_ADV_SEC_PHY_1_MBPS,
  .sid =          0
};

uint8_t advData1[] =
{
  0x02,
  GAP_ADTYPE_FLAGS,
  GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED | GAP_ADTYPE_FLAGS_GENERAL,

  0x03,
  GAP_ADTYPE_16BIT_MORE,
  LO_UINT16(0xfff0),
  HI_UINT16(0xfff0),

};

uint8_t scanResData1[] =
{
  0x12,
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'B',
  'a',
  's',
  'i',
  'c',
  ' ',
  'B',
  'L',
  'E',
  ' ',
  'p',
  'r',
  'o',
  'j',
  'e',
  'c',
  't',

  0x02,
  GAP_ADTYPE_POWER_LEVEL,
  0,

  0x05,
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16(80),
  HI_UINT16(80),
  LO_UINT16(104),
  HI_UINT16(104),

};

