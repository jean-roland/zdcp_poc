/**
 * \file zdc_Main.c
 * \brief zdc protocol module (A)
 * \author LCSF Generator v1.4
 *
 */

// *** Libraries include ***
// Standard lib
// Custom lib
#include "zdc_Main_a.h"
#include "LCSF_Bridge_zdc_a.h"
#include <LCSF_Config.h>
#include <stdint.h>

// *** Definitions ***
// --- Private Macros ---
#define ZDC_SEND_BUFF_SIZE 128

// --- Private Types ---
typedef struct _zdc_info {
    zdc_cmd_payload_t SendCmdPayload;
    uint16_t curr_size;
    uint8_t SendBuffer[ZDC_SEND_BUFF_SIZE];
} zdc_info_t;

// --- Private Function Prototypes ---
// Generated functions
static bool zdcSendCommand(uint_fast16_t cmdName, bool hasPayload);
static bool zdcExecutelist_entities_resp(zdc_cmd_payload_t *pCmdPayload);

// --- Private Variables ---
static zdc_info_t zdcInfo;

static const lcsf_validator_protocol_desc_t lcsf_zdcp_desc = {
    LCSF_ZDC_PROTOCOL_ID,
    &LCSF_zdc_ProtDesc,
    LCSF_Bridge_zdcReceive,
};

// *** End Definitions ***

// *** Private Functions ***

/**
 * \fn static bool zdcSendCommand(uint_fast16_t cmdName, bool hasPayload)
 * \brief Send a command
 *=
 * \param cmdName name of the command to send
 * \param hasPayload indicates if command has a payload or not
 * \return bool: true if operation was a success
 */
static bool zdcSendCommand(uint_fast16_t cmdName, bool hasPayload) {
    if (cmdName >= ZDC_CMD_COUNT) {
        return false;
    }
    int msgSize = 0;
    if (hasPayload) {
        zdc_cmd_payload_t *pCmdPayload = &zdcInfo.SendCmdPayload;
        msgSize = LCSF_Bridge_zdcEncode(cmdName, pCmdPayload, zdcInfo.SendBuffer, ZDC_SEND_BUFF_SIZE);
    } else {
        msgSize = LCSF_Bridge_zdcEncode(cmdName, NULL, zdcInfo.SendBuffer, ZDC_SEND_BUFF_SIZE);
    }
    if (msgSize <= 0) {
        return false;
    }
    zdcInfo.curr_size = msgSize;
    return true;
}

/**
 * \fn static bool zdcExecuteX(void)
 * \brief Execute command X (no payload)
 *
 * \return bool: true if operation was a success
 */

/**
 * \fn static bool zdcExecuteX(zdc_cmd_payload_t *pCmdPayload)
 * \brief Execute command X (with payload)
 *
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */

static bool zdcExecutelist_entities_resp(zdc_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Declare attributes
    // uint8_t *m_list_entities_resp_entity_list = NULL;
    // Retrieve attributes data
    // m_list_entities_resp_entity_list = pCmdPayload->list_entities_resp_payload.p_entity_list;
    // Process data
    return true;
}

// *** Public Functions ***

/**
 * \fn bool zdc_MainInit(uint8_t *pBuffer, size_t buffSize)
 * \brief Initialize the module
 *
 * \return bool: true if operation was a success
 */
bool zdc_MainInit(void) {
    LCSF_ValidatorAddProtocol(0, &lcsf_zdcp_desc);
    LCSF_Bridge_zdcInit();
    return true;
}

bool zdc_encode_keyexpr(uint_fast8_t eid, char *suffix, uint8_t **pBuffer, size_t *buffSize) {
    zdcInfo.SendCmdPayload.set_entity_keyexpr_payload.entity_id = eid;
    zdcInfo.SendCmdPayload.set_entity_keyexpr_payload.p_keyexpr = suffix;
    if (!zdcSendCommand(ZDC_CMD_SET_ENTITY_KEYEXPR, true)) {
        return false;
    }
    *pBuffer = zdcInfo.SendBuffer;
    *buffSize = zdcInfo.curr_size;
    return true;
}

bool zdc_encode_state(uint_fast8_t eid, uint_fast8_t state, uint8_t **pBuffer, size_t *buffSize) {
    zdcInfo.SendCmdPayload.set_entity_state_payload.entity_id = eid;
    zdcInfo.SendCmdPayload.set_entity_state_payload.state = state;
    if (!zdcSendCommand(ZDC_CMD_SET_ENTITY_STATE, true)) {
        return false;
    }
    *pBuffer = zdcInfo.SendBuffer;
    *buffSize = zdcInfo.curr_size;
    return true;
}

// Place custom public functions here

/**
 * \fn bool zdc_MainExecute(uint_fast16_t cmdName, zdc_cmd_payload_t *pCmdPayload)
 * \brief Execute a command
 *
 * \param cmdName name of the command
 * \param pCmdPayload pointer to command payload
 * \return bool: true if operation was a success
 */
bool zdc_MainExecute(uint_fast16_t cmdName, zdc_cmd_payload_t *pCmdPayload) {
    switch (cmdName) {
        case ZDC_CMD_LIST_ENTITIES_RESP:
            return zdcExecutelist_entities_resp(pCmdPayload);

        default:
            // This case can be customized (e.g to send an error command)
            return false;
    }
}
