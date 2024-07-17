/**
 * \file LCSF_Desc_zdc.c
 * \brief zdc LCSF descriptor
 * \author LCSF Generator v1.4
 *
 */

// *** Libraries include ***
// Standard lib
// Custom lib
#include "LCSF_Bridge_zdc_a.h"
#include <LCSF_Config.h>

// *** Definitions ***
// --- Private Constants ---

// Attribute array descriptor of command list_entities_resp
static const lcsf_attribute_desc_t LCSF_zdc_list_entities_resp_AttDescArray[LCSF_ZDC_CMD_LIST_ENTITIES_RESP_ATT_NB] = {
    {false, LCSF_BYTE_ARRAY, LCSF_ZDC_LIST_ENTITIES_RESP_ATT_ID_ENTITY_LIST, 0, NULL},
};

// Attribute array descriptor of command set_entity_state
static const lcsf_attribute_desc_t LCSF_zdc_set_entity_state_AttDescArray[LCSF_ZDC_CMD_SET_ENTITY_STATE_ATT_NB] = {
    {false, LCSF_UINT16, LCSF_ZDC_SET_ENTITY_STATE_ATT_ID_ENTITY_ID, 0, NULL},
    {false, LCSF_UINT8, LCSF_ZDC_SET_ENTITY_STATE_ATT_ID_STATE, 0, NULL},
};

// Attribute array descriptor of command set_entity_keyexpr
static const lcsf_attribute_desc_t LCSF_zdc_set_entity_keyexpr_AttDescArray[LCSF_ZDC_CMD_SET_ENTITY_KEYEXPR_ATT_NB] = {
    {false, LCSF_UINT16, LCSF_ZDC_SET_ENTITY_KEYEXPR_ATT_ID_ENTITY_ID, 0, NULL},
    {false, LCSF_STRING, LCSF_ZDC_SET_ENTITY_KEYEXPR_ATT_ID_KEYEXPR, 0, NULL},
};

// Attribute array descriptor of command set_entity_config
static const lcsf_attribute_desc_t LCSF_zdc_set_entity_config_AttDescArray[LCSF_ZDC_CMD_SET_ENTITY_CONFIG_ATT_NB] = {
    {false, LCSF_UINT16, LCSF_ZDC_SET_ENTITY_CONFIG_ATT_ID_ENTITY_ID, 0, NULL},
    {false, LCSF_BYTE_ARRAY, LCSF_ZDC_SET_ENTITY_CONFIG_ATT_ID_ENTITIY_CONFIG, 0, NULL},
};

// Command array descriptor
static const lcsf_command_desc_t LCSF_zdc_CmdDescArray[LCSF_ZDC_CMD_NB] = {
    {LCSF_ZDC_CMD_ID_LIST_ENTITIES_REQ, 0, NULL},
    {LCSF_ZDC_CMD_ID_LIST_ENTITIES_RESP, LCSF_ZDC_CMD_LIST_ENTITIES_RESP_ATT_NB, LCSF_zdc_list_entities_resp_AttDescArray},
    {LCSF_ZDC_CMD_ID_SET_ENTITY_STATE, LCSF_ZDC_CMD_SET_ENTITY_STATE_ATT_NB, LCSF_zdc_set_entity_state_AttDescArray},
    {LCSF_ZDC_CMD_ID_SET_ENTITY_KEYEXPR, LCSF_ZDC_CMD_SET_ENTITY_KEYEXPR_ATT_NB, LCSF_zdc_set_entity_keyexpr_AttDescArray},
    {LCSF_ZDC_CMD_ID_SET_ENTITY_CONFIG, LCSF_ZDC_CMD_SET_ENTITY_CONFIG_ATT_NB, LCSF_zdc_set_entity_config_AttDescArray},
};

// --- Public Constants ---

// Protocol descriptor
const lcsf_protocol_desc_t LCSF_zdc_ProtDesc = {.CmdNb = LCSF_ZDC_CMD_NB, .pCmdDescArray = LCSF_zdc_CmdDescArray};
