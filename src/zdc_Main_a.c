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
#include <stdio.h>
#include <zenoh-pico.h>

// *** Definitions ***
// --- Private Macros ---
#define ZDC_SEND_BUFF_SIZE 128

// --- Private Types ---
enum _zdc_state_e {
    ZDC_STATE_OFF = 0,
    ZDC_STATE_ON = 1,
    ZDC_STATE_INVALID = 2,
};

enum _zdc_type_e {
    ZDC_TYPE_PUB = 0,
    ZDC_TYPE_SUB = 1,
    ZDC_TYPE_QUERY = 2,
    ZDC_TYPE_QUERYABLE = 3,
};

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

static const char *ZDC_STATE_TO_STR[] = {"Off", "On", "Invalid"};
static const char *ZDC_TYPE_TO_STR[] = {"Pub", "Sub", "Querier", "Queryable"};

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
    // Retrieve attributes data
    uint8_t *m_entity_list = pCmdPayload->list_entities_resp_payload.p_entity_list;
    uint16_t m_entity_nb = pCmdPayload->list_entities_resp_payload.entity_nb;
    // Process data
    printf("Entities list (%d):\n", m_entity_nb);
    size_t curr_idx = 0;
    for (uint32_t i = 0; i < m_entity_nb; i++) {
        uint8_t type = m_entity_list[curr_idx++];
        uint8_t state = m_entity_list[curr_idx++];
        uint8_t ke_size = m_entity_list[curr_idx++];
        char *ke_suffix = (char *)&m_entity_list[curr_idx];
        printf("Entity %d: type: %s, state: %s, ke: %.*s\n", i, ZDC_TYPE_TO_STR[type], ZDC_STATE_TO_STR[state], (int)ke_size,
            ke_suffix);
        curr_idx += ke_size;
    }
    return true;
}

static bool zdcExecutecmd_status(zdc_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Declare attributes
    uint8_t m_cmd_status_status_value = 0;
    // Retrieve attributes data
    m_cmd_status_status_value = pCmdPayload->cmd_status_payload.status_value;
    // Process data
    printf("Command status %s\n", (m_cmd_status_status_value == 0) ? "Ok" : "Err");
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

bool zdc_encode_list_entity(uint8_t **pBuffer, size_t *buffSize) {
    if (!zdcSendCommand(ZDC_CMD_LIST_ENTITIES_REQ, false)) {
        return false;
    }
    *pBuffer = zdcInfo.SendBuffer;
    *buffSize = zdcInfo.curr_size;
    return true;
}

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

        case ZDC_CMD_CMD_STATUS:
            return zdcExecutecmd_status(pCmdPayload);

        default:
            // This case can be customized (e.g to send an error command)
            return false;
    }
}
