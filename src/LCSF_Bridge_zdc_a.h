/**
 * \file LCSF_Bridge_zdc.h
 * \brief zdc LCSF bridge module
 * \author LCSF Generator v1.4
 *
 */

#ifndef Lcsf_bridge_zdc_h
#define Lcsf_bridge_zdc_h

// *** Libraries include ***
// Standard lib
// Custom lib
#include "zdc_Main_a.h"
#include <LCSF_Config.h>
#include <lib/LCSF_Validator.h>

// *** Definitions ***
// --- Public Types ---

// Command identifier enum
enum _lcsf_zdc_cmd_id {
    LCSF_ZDC_CMD_ID_LIST_ENTITIES_REQ = 0x0,
    LCSF_ZDC_CMD_ID_LIST_ENTITIES_RESP = 0x1,
    LCSF_ZDC_CMD_ID_SET_ENTITY_STATE = 0x2,
    LCSF_ZDC_CMD_ID_SET_ENTITY_KEYEXPR = 0x3,
    LCSF_ZDC_CMD_ID_SET_ENTITY_CONFIG = 0x4,
};

// Attribute identifier enums
enum _lcsf_zdc_list_entities_resp_att_id {
    LCSF_ZDC_LIST_ENTITIES_RESP_ATT_ID_ENTITY_LIST = 0x0,
};

enum _lcsf_zdc_set_entity_config_att_id {
    LCSF_ZDC_SET_ENTITY_CONFIG_ATT_ID_ENTITY_ID = 0x0,
    LCSF_ZDC_SET_ENTITY_CONFIG_ATT_ID_ENTITIY_CONFIG = 0x1,
};

enum _lcsf_zdc_set_entity_keyexpr_att_id {
    LCSF_ZDC_SET_ENTITY_KEYEXPR_ATT_ID_ENTITY_ID = 0x0,
    LCSF_ZDC_SET_ENTITY_KEYEXPR_ATT_ID_KEYEXPR = 0x1,
};

enum _lcsf_zdc_set_entity_state_att_id {
    LCSF_ZDC_SET_ENTITY_STATE_ATT_ID_ENTITY_ID = 0x0,
    LCSF_ZDC_SET_ENTITY_STATE_ATT_ID_STATE = 0x1,
};

// --- Public Constants ---

// Bridge decoder filo size
#define LCSF_BRIDGE_ZDC_FILO_SIZE 2
// Lcsf protocol identifier
#define LCSF_ZDC_PROTOCOL_ID 0x2e
// Command number
#define LCSF_ZDC_CMD_NB ZDC_CMD_COUNT
// Command attribute number
#define LCSF_ZDC_CMD_LIST_ENTITIES_RESP_ATT_NB 1
#define LCSF_ZDC_CMD_SET_ENTITY_STATE_ATT_NB 2
#define LCSF_ZDC_CMD_SET_ENTITY_KEYEXPR_ATT_NB 2
#define LCSF_ZDC_CMD_SET_ENTITY_CONFIG_ATT_NB 2

// Protocol descriptor
extern const lcsf_protocol_desc_t LCSF_zdc_ProtDesc;

// --- Public Function Prototypes ---

/**
 * \fn bool LCSF_Bridge_zdcInit(void)
 * \brief Initialize the module
 *
 * \return bool: true if operation was a success
 */
bool LCSF_Bridge_zdcInit(void);

/**
 * \fn bool LCSF_Bridge_zdcReceive(lcsf_valid_cmd_t *pValidCmd)
 * \brief Receive valid command from LCSF_Validator and transmit to zdc_Main
 *
 * \param pValidCmd pointer to the valid command
 * \return bool: true if operation was a success
 */
bool LCSF_Bridge_zdcReceive(lcsf_valid_cmd_t *pValidCmd);

/**
 * \fn int LCSF_Bridge_zdcEncode(uint_fast16_t cmdName, zdc_cmd_payload_t *pCmdPayload, uint8_t *pBuffer, size_t buffSize)
 * \brief Receive command from zdc_Main and transmit to LCSF_Validator for encoding
 *
 * \param cmdName name of the command
 * \param pValidCmd pointer to the valid command
 * \param pBuffer pointer to the send buffer
 * \param buffSize buffer size
 * \return int: -1 if operation failed, encoded message size if success
 */
int LCSF_Bridge_zdcEncode(uint_fast16_t cmdName, zdc_cmd_payload_t *pCmdPayload, uint8_t *pBuffer, size_t buffSize);

// *** End Definitions ***
#endif // Lcsf_bridge_zdc_h
