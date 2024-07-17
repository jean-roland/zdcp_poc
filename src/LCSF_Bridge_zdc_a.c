/**
 * \file LCSF_Bridge_zdc.c
 * \brief zdc LCSF bridge module (A)
 * \author LCSF Generator v1.4
 *
 */

// *** Libraries include ***
// Standard lib
#include <string.h>
// Custom lib
#include "LCSF_Bridge_zdc_a.h"
#include <LCSF_Config.h>
#include <lib/Filo.h>
#include <lib/LCSF_Transcoder.h>
#include <lib/LCSF_Validator.h>

// *** Definitions ***
// --- Private Types ---

// Module information structure
typedef struct _lcsf_bridge_zdc_info {
    uint8_t FiloData[LCSF_BRIDGE_ZDC_FILO_SIZE * sizeof(lcsf_valid_att_t)];
    filo_desc_t Filo;
    zdc_cmd_payload_t CmdPayload;
} lcsf_bridge_zdc_info_t;

// --- Private Constants ---
// Array to convert command name value to their lcsf command id
static const uint16_t LCSF_Bridge_zdc_CMDNAME2CMDID[LCSF_ZDC_CMD_NB] = {
    LCSF_ZDC_CMD_ID_LIST_ENTITIES_REQ,
    LCSF_ZDC_CMD_ID_LIST_ENTITIES_RESP,
    LCSF_ZDC_CMD_ID_SET_ENTITY_STATE,
    LCSF_ZDC_CMD_ID_SET_ENTITY_KEYEXPR,
    LCSF_ZDC_CMD_ID_SET_ENTITY_CONFIG,
};

// --- Private Function Prototypes ---
static uint16_t LCSF_Bridge_zdc_CMDID2CMDNAME(uint_fast16_t cmdId);
static void LCSF_Bridge_zdclist_entities_respGetData(lcsf_valid_att_t *pAttArray, zdc_cmd_payload_t *pCmdPayload);
static void LCSF_Bridge_zdcGetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, zdc_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_zdcset_entity_stateFillAtt(lcsf_valid_att_t **pAttArrayAddr, zdc_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_zdcset_entity_keyexprFillAtt(lcsf_valid_att_t **pAttArrayAddr, zdc_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_zdcset_entity_configFillAtt(lcsf_valid_att_t **pAttArrayAddr, zdc_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_zdcFillCmdAtt(
    uint_fast16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, zdc_cmd_payload_t *pCmdPayload);

// --- Private Variables ---
static lcsf_bridge_zdc_info_t LcsfBridgezdcInfo;

// *** End Definitions ***

// *** Private Functions ***

/**
 * \fn static uint16_t LCSF_Bridge_zdc_CMDID2CMDNAME(uint_fast16_t cmdId)
 * \brief Translate an lcsf command id to its name value
 *
 * \param cmdId lcsf command identifier to translate
 * \return uint16_t: name value of the command
 */
static uint16_t LCSF_Bridge_zdc_CMDID2CMDNAME(uint_fast16_t cmdId) {
    switch (cmdId) {
        default:
        case LCSF_ZDC_CMD_ID_LIST_ENTITIES_REQ:
            return ZDC_CMD_LIST_ENTITIES_REQ;
        case LCSF_ZDC_CMD_ID_LIST_ENTITIES_RESP:
            return ZDC_CMD_LIST_ENTITIES_RESP;
        case LCSF_ZDC_CMD_ID_SET_ENTITY_STATE:
            return ZDC_CMD_SET_ENTITY_STATE;
        case LCSF_ZDC_CMD_ID_SET_ENTITY_KEYEXPR:
            return ZDC_CMD_SET_ENTITY_KEYEXPR;
        case LCSF_ZDC_CMD_ID_SET_ENTITY_CONFIG:
            return ZDC_CMD_SET_ENTITY_CONFIG;
    }
}

/**
 * \fn static void LCSF_Bridge_zdcXGetData(lcsf_valid_att_t *pAttArray, zdc_cmd_payload_t *pCmdPayload)
 * \brief Retrieve data of command X from its valid attribute array and store it in a payload
 *
 * \param pAttArray pointer to the command attribute array
 * \param pCmdPayload pointer to the payload to contain the command data
 * \return void
 */
static void LCSF_Bridge_zdclist_entities_respGetData(lcsf_valid_att_t *pAttArray, zdc_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return;
    }
    // Retrieve data of attribute entity_list
    pCmdPayload->list_entities_resp_payload.p_entity_list = pAttArray[ZDC_LIST_ENTITIES_RESP_ATT_ENTITY_LIST].Payload.pData;
}

/**
 * \fn static void LCSF_Bridge_zdcGetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, zdc_cmd_payload_t *pCmdPayload)
 * \brief Retrieve command data from its attribute array and store it in a payload
 *
 * \param cmdName name of the command
 * \param pAttArray pointer to the command attribute array
 * \param pPayload pointer to the payload to contain the command data
 * \return void
 */
static void LCSF_Bridge_zdcGetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, zdc_cmd_payload_t *pCmdPayload) {
    if (pAttArray == NULL) {
        return;
    }
    switch (cmdName) {
        case ZDC_CMD_LIST_ENTITIES_RESP:
            LCSF_Bridge_zdclist_entities_respGetData(pAttArray, pCmdPayload);
            break;

        default: // Commands that don't have payload
            return;
    }
}

/**
 * \fn static bool LCSF_Bridge_zdcXFillAtt(lcsf_valid_att_t **pAttArrayAddr, zdc_cmd_payload_t *pCmdPayload)
 * \brief Allocate and fill attribute array of command X from its payload
 *
 * \param pAttArrayAddr pointer to contain the attribute array
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */
static bool LCSF_Bridge_zdcset_entity_stateFillAtt(lcsf_valid_att_t **pAttArrayAddr, zdc_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Allocate attribute array
    if (!FiloGet(&LcsfBridgezdcInfo.Filo, LCSF_ZDC_CMD_SET_ENTITY_STATE_ATT_NB, (void *)pAttArrayAddr)) {
        return false;
    }
    // Intermediary variable
    lcsf_valid_att_t *pAttArray = *pAttArrayAddr;
    // Fill data of attribute entity_id
    pAttArray[ZDC_SET_ENTITY_STATE_ATT_ENTITY_ID].PayloadSize = GetVLESize(pCmdPayload->set_entity_state_payload.entity_id);
    pAttArray[ZDC_SET_ENTITY_STATE_ATT_ENTITY_ID].Payload.pData = &(pCmdPayload->set_entity_state_payload.entity_id);
    // Fill data of attribute state
    pAttArray[ZDC_SET_ENTITY_STATE_ATT_STATE].PayloadSize = GetVLESize(pCmdPayload->set_entity_state_payload.state);
    pAttArray[ZDC_SET_ENTITY_STATE_ATT_STATE].Payload.pData = &(pCmdPayload->set_entity_state_payload.state);
    return true;
}

static bool LCSF_Bridge_zdcset_entity_keyexprFillAtt(lcsf_valid_att_t **pAttArrayAddr, zdc_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Allocate attribute array
    if (!FiloGet(&LcsfBridgezdcInfo.Filo, LCSF_ZDC_CMD_SET_ENTITY_KEYEXPR_ATT_NB, (void *)pAttArrayAddr)) {
        return false;
    }
    // Intermediary variable
    lcsf_valid_att_t *pAttArray = *pAttArrayAddr;
    // Fill data of attribute entity_id
    pAttArray[ZDC_SET_ENTITY_KEYEXPR_ATT_ENTITY_ID].PayloadSize =
        GetVLESize(pCmdPayload->set_entity_keyexpr_payload.entity_id);
    pAttArray[ZDC_SET_ENTITY_KEYEXPR_ATT_ENTITY_ID].Payload.pData = &(pCmdPayload->set_entity_keyexpr_payload.entity_id);
    // Fill data of attribute keyexpr
    pAttArray[ZDC_SET_ENTITY_KEYEXPR_ATT_KEYEXPR].Payload.pData = pCmdPayload->set_entity_keyexpr_payload.p_keyexpr;
    return true;
}

static bool LCSF_Bridge_zdcset_entity_configFillAtt(lcsf_valid_att_t **pAttArrayAddr, zdc_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Allocate attribute array
    if (!FiloGet(&LcsfBridgezdcInfo.Filo, LCSF_ZDC_CMD_SET_ENTITY_CONFIG_ATT_NB, (void *)pAttArrayAddr)) {
        return false;
    }
    // Intermediary variable
    lcsf_valid_att_t *pAttArray = *pAttArrayAddr;
    // Fill data of attribute entity_id
    pAttArray[ZDC_SET_ENTITY_CONFIG_ATT_ENTITY_ID].PayloadSize =
        GetVLESize(pCmdPayload->set_entity_config_payload.entity_id);
    pAttArray[ZDC_SET_ENTITY_CONFIG_ATT_ENTITY_ID].Payload.pData = &(pCmdPayload->set_entity_config_payload.entity_id);
    // Fill data of attribute entitiy_config
    pAttArray[ZDC_SET_ENTITY_CONFIG_ATT_ENTITIY_CONFIG].PayloadSize =
        pCmdPayload->set_entity_config_payload.entitiy_configSize;
    pAttArray[ZDC_SET_ENTITY_CONFIG_ATT_ENTITIY_CONFIG].Payload.pData =
        pCmdPayload->set_entity_config_payload.p_entitiy_config;
    return true;
}

/**
 * \fn static bool LCSF_Bridge_zdcFillCmdAtt(uint_fast16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, zdc_cmd_payload_t *pCmdPayload)
 * \brief Fill the attribute array of a command from its payload
 *
 * \param cmdName name of the command
 * \param pAttArrayAddr pointer to contain the attribute array
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */
static bool LCSF_Bridge_zdcFillCmdAtt(
    uint_fast16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, zdc_cmd_payload_t *pCmdPayload) {
    switch (cmdName) {
        case ZDC_CMD_SET_ENTITY_STATE:
            return LCSF_Bridge_zdcset_entity_stateFillAtt(pAttArrayAddr, pCmdPayload);

        case ZDC_CMD_SET_ENTITY_KEYEXPR:
            return LCSF_Bridge_zdcset_entity_keyexprFillAtt(pAttArrayAddr, pCmdPayload);

        case ZDC_CMD_SET_ENTITY_CONFIG:
            return LCSF_Bridge_zdcset_entity_configFillAtt(pAttArrayAddr, pCmdPayload);

        default: // Commands that don't have attributes
            *pAttArrayAddr = NULL;
            return true;
    }
}

// *** Public Functions ***

bool LCSF_Bridge_zdcInit(void) {
    return FiloInit(
        &LcsfBridgezdcInfo.Filo, LcsfBridgezdcInfo.FiloData, LCSF_BRIDGE_ZDC_FILO_SIZE, sizeof(lcsf_valid_att_t));
}

bool LCSF_Bridge_zdcReceive(lcsf_valid_cmd_t *pValidCmd) {
    uint16_t cmdName = LCSF_Bridge_zdc_CMDID2CMDNAME(pValidCmd->CmdId);
    zdc_cmd_payload_t *pCmdPayload = &LcsfBridgezdcInfo.CmdPayload;

    LCSF_Bridge_zdcGetCmdData(cmdName, pValidCmd->pAttArray, pCmdPayload);
    return zdc_MainExecute(cmdName, pCmdPayload);
}

int LCSF_Bridge_zdcEncode(uint_fast16_t cmdName, zdc_cmd_payload_t *pCmdPayload, uint8_t *pBuffer, size_t buffSize) {
    lcsf_valid_cmd_t sendCmd;
    sendCmd.CmdId = LCSF_Bridge_zdc_CMDNAME2CMDID[cmdName];
    FiloFreeAll(&LcsfBridgezdcInfo.Filo);

    if (!LCSF_Bridge_zdcFillCmdAtt(cmdName, &(sendCmd.pAttArray), pCmdPayload)) {
        return -1;
    }
    return LCSF_ValidatorEncode(LCSF_ZDC_PROTOCOL_ID, &sendCmd, pBuffer, buffSize);
}
