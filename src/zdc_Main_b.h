/**
 * \file zdc_Main.h
 * \brief zdc protocol module
 * \author LCSF Generator v1.4
 *
 */

#ifndef zdc_main_h
#define zdc_main_h

// *** Libraries include ***
// Standard lib
// Custom lib
#include <LCSF_Config.h>
#include <zenoh-pico.h>

// *** Definitions ***
// --- Public Types ---

// Command name enum
enum _zdc_cmd_names {
    ZDC_CMD_LIST_ENTITIES_REQ,
    ZDC_CMD_LIST_ENTITIES_RESP,
    ZDC_CMD_SET_ENTITY_STATE,
    ZDC_CMD_SET_ENTITY_KEYEXPR,
    ZDC_CMD_SET_ENTITY_CONFIG,
    ZDC_CMD_COUNT,
};

// Attributes enums
enum _zdc_list_entities_resp_att_names {
    ZDC_LIST_ENTITIES_RESP_ATT_ENTITY_LIST,
};

enum _zdc_set_entity_config_att_names {
    ZDC_SET_ENTITY_CONFIG_ATT_ENTITY_ID,
    ZDC_SET_ENTITY_CONFIG_ATT_ENTITIY_CONFIG,
};

enum _zdc_set_entity_keyexpr_att_names {
    ZDC_SET_ENTITY_KEYEXPR_ATT_ENTITY_ID,
    ZDC_SET_ENTITY_KEYEXPR_ATT_KEYEXPR,
};

enum _zdc_set_entity_state_att_names {
    ZDC_SET_ENTITY_STATE_ATT_ENTITY_ID,
    ZDC_SET_ENTITY_STATE_ATT_STATE,
};

// Commands data structures
typedef struct _zdc_list_entities_resp_att_payload {
    uint32_t entity_listSize;
    uint8_t *p_entity_list;
} zdc_list_entities_resp_att_payload_t;

typedef struct _zdc_set_entity_state_att_payload {
    uint16_t entity_id;
    uint8_t state;
} zdc_set_entity_state_att_payload_t;

typedef struct _zdc_set_entity_keyexpr_att_payload {
    uint16_t entity_id;
    char *p_keyexpr;
} zdc_set_entity_keyexpr_att_payload_t;

typedef struct _zdc_set_entity_config_att_payload {
    uint16_t entity_id;
    uint32_t entitiy_configSize;
    uint8_t *p_entitiy_config;
} zdc_set_entity_config_att_payload_t;

// Command payload union
typedef union _zdc_cmd_payload {
    zdc_list_entities_resp_att_payload_t list_entities_resp_payload;
    zdc_set_entity_state_att_payload_t set_entity_state_payload;
    zdc_set_entity_keyexpr_att_payload_t set_entity_keyexpr_payload;
    zdc_set_entity_config_att_payload_t set_entity_config_payload;
} zdc_cmd_payload_t;

// --- Public Function Prototypes ---

/**
 * \fn bool zdc_MainInit(uint8_t *pBuffer, size_t buffSize)
 * \brief Initialize the module
 *
 * \return bool: true if operation was a success
 */
bool zdc_MainInit(const z_loaned_session_t *zs);

void zdc_close(void);

int zdc_entity_state(uint_fast8_t eid);

char * zdc_entity_ke_suffix(uint_fast8_t eid);

/**
 * \fn bool zdc_MainExecute(uint_fast16_t cmdName, zdc_cmd_payload_t *pCmdPayload)
 * \brief Execute a command
 *
 * \param cmdName name of the command
 * \param pCmdPayload pointer to command payload
 * \return bool: true if operation was a success
 */
bool zdc_MainExecute(uint_fast16_t cmdName, zdc_cmd_payload_t *pCmdPayload);

// *** End Definitions ***
#endif // zdc_Main_h
