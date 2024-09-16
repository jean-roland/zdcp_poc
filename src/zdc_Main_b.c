/**
 * \file zdc_Main.c
 * \brief zdc protocol module (B)
 * \author LCSF Generator v1.4
 *
 */

// *** Libraries include ***
// Standard lib
// Custom lib
#include "zdc_Main_b.h"
#include "LCSF_Bridge_zdc_b.h"
#include <LCSF_Config.h>
#include <lib/LCSF_Transcoder.h>
#include <stdlib.h>

// *** Definitions ***
// --- Private Macros ---
#define ZDC_ENTITY_ID_SELF 0

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

typedef void (*zdc_data_handler_t)(z_loaned_sample_t *sample, void *arg);
typedef void (*zdc_queryable_handler_t)(z_loaned_query_t *query, void *arg);

typedef struct _zdc_pub_entity_t {
    z_put_options_t *config;
} zdc_pub_entity_t;

typedef struct _zdc_sub_entity_t {
    zdc_data_handler_t cb_ptr;
    z_subscriber_options_t *config;
    z_owned_subscriber_t data;
} zdc_sub_entity_t;

typedef struct _zdc_query_entity_t {
    z_get_options_t *config;
} zdc_query_entity_t;

typedef struct _zdc_queryable_entity_t {
    zdc_queryable_handler_t cb_ptr;
    z_queryable_options_t *config;
    z_owned_queryable_t data;
} zdc_queryable_entity_t;

typedef struct _zdc_entity_t {
    char *ke_suffix;
    union {
        zdc_pub_entity_t pub;
        zdc_sub_entity_t sub;
        zdc_query_entity_t query;
        zdc_queryable_entity_t queryable;
    } body;
    enum _zdc_type_e type;
    size_t ke_size;
    int8_t state;
} zdc_entity_t;

typedef struct _zdc_info {
    uint8_t *pSendBuffer;
    uint16_t buff_size;
    uint16_t el_size;
    zdc_cmd_payload_t SendCmdPayload;
    uint8_t el_idx;
    zdc_entity_t *entity_list;
    const z_loaned_session_t *session;
} zdc_info_t;

// --- Private Function Prototypes ---
// Generated functions
static bool zdcSendCommand(uint_fast16_t cmdName, bool hasPayload);
static bool zdcExecutelist_entities_req(void);
static bool zdcExecuteset_entity_state(zdc_cmd_payload_t *pCmdPayload);
static bool zdcExecuteset_entity_keyexpr(zdc_cmd_payload_t *pCmdPayload);
static bool zdcExecuteset_entity_config(zdc_cmd_payload_t *pCmdPayload);

// --- Private Variables ---
static zdc_info_t zdcInfo;

static const lcsf_validator_protocol_desc_t lcsf_zdcp_desc = {
    LCSF_ZDC_PROTOCOL_ID,
    &LCSF_zdc_ProtDesc,
    LCSF_Bridge_zdcReceive,
};

// *** End Definitions ***

// *** Private Functions ***

static void zdc_cmd_handler(z_loaned_sample_t *sample, void *ctx) {
    (void)(ctx);
    z_owned_slice_t value;
    z_bytes_deserialize_into_slice(z_sample_payload(sample), &value);
    printf("Received some command!\n");
    LCSF_TranscoderReceive(z_slice_data(z_loan(value)), z_slice_len(z_loan(value)));
    z_drop(z_move(value));
}

static bool zdc_update_entity_state(size_t e_id, uint_fast8_t state) {
    zdc_entity_t *entity = &zdcInfo.entity_list[e_id];
    if (entity->state == state) {
        return true;
    }
    if ((entity->state == ZDC_STATE_INVALID) && (state == ZDC_STATE_OFF)) {
        entity->state = state;
        return true;
    }
    entity->state = ZDC_STATE_INVALID;
    if (state == ZDC_STATE_ON) {
        switch (entity->type) {
            case ZDC_TYPE_PUB:
                break;
            case ZDC_TYPE_SUB:
                {
                    z_owned_closure_sample_t callback;
                    z_closure(&callback, entity->body.sub.cb_ptr);
                    z_view_keyexpr_t ke;
                    z_view_keyexpr_from_str(&ke, entity->ke_suffix);
                    z_declare_subscriber(
                        &entity->body.sub.data, zdcInfo.session, z_loan(ke), z_move(callback), entity->body.sub.config);
                }
                break;
            case ZDC_TYPE_QUERY:
                break;
            case ZDC_TYPE_QUERYABLE:
                {
                    z_owned_closure_query_t callback;
                    z_closure(&callback, entity->body.queryable.cb_ptr);
                    z_view_keyexpr_t ke;
                    z_view_keyexpr_from_str(&ke, entity->ke_suffix);
                    z_declare_queryable(&entity->body.queryable.data, zdcInfo.session, z_loan(ke), z_move(callback),
                        entity->body.queryable.config);
                }
                break;
            default:
                return false;
        }
    } else if (state == ZDC_STATE_OFF) {
        switch (entity->type) {
            case ZDC_TYPE_PUB:
                break;
            case ZDC_TYPE_SUB:
                z_undeclare_subscriber(z_move(entity->body.sub.data));
                break;
            case ZDC_TYPE_QUERY:
                break;
            case ZDC_TYPE_QUERYABLE:
                z_undeclare_queryable(z_move(entity->body.queryable.data));
                break;
            default:
                return false;
        }
    }
    entity->state = state;
    return true;
}

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
        msgSize = LCSF_Bridge_zdcEncode(cmdName, pCmdPayload, zdcInfo.pSendBuffer, zdcInfo.buff_size);
    } else {
        msgSize = LCSF_Bridge_zdcEncode(cmdName, NULL, zdcInfo.pSendBuffer, zdcInfo.buff_size);
    }
    if (msgSize <= 0) {
        return false;
    }
    // TODO Pass buffer to send function
    return true;
}

static bool zdcExecutelist_entities_req(void) {
    printf("Listing entities\n");
    zdcInfo.SendCmdPayload.list_entities_resp_payload.p_entity_list = (uint8_t *)zdcInfo.entity_list;
    zdcInfo.SendCmdPayload.list_entities_resp_payload.entity_listSize = zdcInfo.el_size * sizeof(zdc_entity_t);
    zdcSendCommand(ZDC_CMD_LIST_ENTITIES_RESP, true);
    return true;
}

static bool zdcExecuteset_entity_state(zdc_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Retrieve attributes data
    uint16_t e_id = pCmdPayload->set_entity_state_payload.entity_id;
    uint8_t state = pCmdPayload->set_entity_state_payload.state;
    // Process data
    if (e_id >= zdcInfo.el_size) {
        return false;
    }
    printf("Updating state: %d, %d\n", e_id, state);
    // Set state
    return zdc_update_entity_state(e_id, state);
}

static bool zdcExecuteset_entity_keyexpr(zdc_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Retrieve attributes data
    uint16_t e_id = pCmdPayload->set_entity_keyexpr_payload.entity_id;
    char *ke = pCmdPayload->set_entity_keyexpr_payload.p_keyexpr;
    // Process data
    if (e_id >= zdcInfo.el_size) {
        return false;
    }
    // Update entity
    printf("Updating keyexpr: %d, %s\n", e_id, ke);
    _Bool restart = false;
    if (zdcInfo.entity_list[e_id].state == ZDC_STATE_ON) {
        restart = true;
        zdc_update_entity_state(e_id, ZDC_STATE_OFF);
    }
    size_t new_size = strlen(ke) + 1;
    // Realloc if needed
    if (new_size > zdcInfo.entity_list[e_id].ke_size) {
        zdcInfo.entity_list[e_id].ke_suffix = (char *)realloc(zdcInfo.entity_list[e_id].ke_suffix, new_size);
        zdcInfo.entity_list[e_id].ke_size = new_size;
    }
    strcpy(zdcInfo.entity_list[e_id].ke_suffix, ke);
    if (restart) {
        zdc_update_entity_state(e_id, ZDC_STATE_ON);
    }
    return true;
}

static bool zdcExecuteset_entity_config(zdc_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Retrieve attributes data
    uint16_t e_id = pCmdPayload->set_entity_config_payload.entity_id;
    // void *config = pCmdPayload->set_entity_config_payload.p_entitiy_config;
    // Process data
    if (e_id >= zdcInfo.el_size) {
        return false;
    }
    // Update entity
    printf("(TODO) Updating config: %d\n", e_id);
    // zdc_update_entity_state(e_id, ZDC_STATE_OFF);
    // zdcInfo.entity_list[e_id].config = config;
    // zdc_update_entity_state(e_id, ZDC_STATE_ON);
    return true;
}

// *** Public Functions ***

/**
 * \fn bool zdc_MainInit(uint8_t *pBuffer, size_t buffSize)
 * \brief Initialize the module
 *
 * \return bool: true if operation was a success
 */
bool zdc_MainInit(const z_loaned_session_t *zs, size_t entity_nb, size_t buff_size, char *zdc_suffix) {
    LCSF_ValidatorAddProtocol(0, &lcsf_zdcp_desc);
    LCSF_Bridge_zdcInit();
    zdcInfo.el_idx = 0;
    zdcInfo.el_size = entity_nb + 1;
    zdcInfo.entity_list = (zdc_entity_t *)malloc(sizeof(zdc_entity_t) * zdcInfo.el_size);
    zdcInfo.buff_size = buff_size;
    zdcInfo.pSendBuffer = (uint8_t *)malloc(zdcInfo.buff_size);

    // Clone zenoh session
    zdcInfo.session = zs;
    // Init zdcp sub entity
    zdcInfo.entity_list[zdcInfo.el_idx].type = ZDC_TYPE_SUB;
    zdcInfo.entity_list[zdcInfo.el_idx].state = ZDC_STATE_OFF;
    zdcInfo.entity_list[zdcInfo.el_idx].ke_size = strlen(zdc_suffix) + 1;
    zdcInfo.entity_list[zdcInfo.el_idx].ke_suffix = (char *)malloc(zdcInfo.entity_list[zdcInfo.el_idx].ke_size);
    strcpy(zdcInfo.entity_list[zdcInfo.el_idx].ke_suffix, zdc_suffix);
    zdcInfo.entity_list[zdcInfo.el_idx].body.sub.config = NULL;
    zdcInfo.entity_list[zdcInfo.el_idx].body.sub.cb_ptr = zdc_cmd_handler;
    zdc_update_entity_state(zdcInfo.el_idx, ZDC_STATE_ON);
    zdcInfo.el_idx++;

    return true;
}

void zdc_add_pub_entity(_Bool start_on, char *ke_suffix, z_put_options_t *config) {
    zdcInfo.entity_list[zdcInfo.el_idx].type = ZDC_TYPE_PUB;
    zdcInfo.entity_list[zdcInfo.el_idx].state = ZDC_STATE_INVALID;
    zdcInfo.entity_list[zdcInfo.el_idx].ke_size = strlen(ke_suffix) + 1;
    zdcInfo.entity_list[zdcInfo.el_idx].ke_suffix = (char *)malloc(zdcInfo.entity_list[zdcInfo.el_idx].ke_size);
    strcpy(zdcInfo.entity_list[zdcInfo.el_idx].ke_suffix, ke_suffix);

    zdcInfo.entity_list[zdcInfo.el_idx].body.pub.config = config;

    if (start_on) {
        zdc_update_entity_state(zdcInfo.el_idx, ZDC_STATE_ON);
    } else {
        zdc_update_entity_state(zdcInfo.el_idx, ZDC_STATE_OFF);
    }
    zdcInfo.el_idx++;
}

void zdc_add_sub_entity(_Bool start_on, char *ke_suffix, z_subscriber_options_t *config, zdc_data_handler_t cb_ptr) {
    zdcInfo.entity_list[zdcInfo.el_idx].type = ZDC_TYPE_SUB;
    zdcInfo.entity_list[zdcInfo.el_idx].state = ZDC_STATE_INVALID;
    zdcInfo.entity_list[zdcInfo.el_idx].ke_size = strlen(ke_suffix) + 1;
    zdcInfo.entity_list[zdcInfo.el_idx].ke_suffix = (char *)malloc(zdcInfo.entity_list[zdcInfo.el_idx].ke_size);
    strcpy(zdcInfo.entity_list[zdcInfo.el_idx].ke_suffix, ke_suffix);

    zdcInfo.entity_list[zdcInfo.el_idx].body.sub.config = config;
    zdcInfo.entity_list[zdcInfo.el_idx].body.sub.cb_ptr = cb_ptr;

    if (start_on) {
        zdc_update_entity_state(zdcInfo.el_idx, ZDC_STATE_ON);
    } else {
        zdc_update_entity_state(zdcInfo.el_idx, ZDC_STATE_OFF);
    }
    zdcInfo.el_idx++;
}

void zdc_close(void) {
    // Stop entities
    for (size_t i = 0; i < zdcInfo.el_size; i++) {
        if (zdcInfo.entity_list[i].state == ZDC_STATE_ON) {
            zdc_update_entity_state(i, ZDC_STATE_OFF);
        }
        free(zdcInfo.entity_list[i].ke_suffix);
    }
}

int zdc_entity_state(uint_fast8_t eid) {
    if (eid >= zdcInfo.el_size) {
        return -1;
    }
    return (int)zdcInfo.entity_list[eid].state;
}

char *zdc_entity_ke_suffix(uint_fast8_t eid) {
    if (eid >= zdcInfo.el_size) {
        return NULL;
    }
    return zdcInfo.entity_list[eid].ke_suffix;
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
        case ZDC_CMD_LIST_ENTITIES_REQ:
            return zdcExecutelist_entities_req();

        case ZDC_CMD_SET_ENTITY_STATE:
            return zdcExecuteset_entity_state(pCmdPayload);

        case ZDC_CMD_SET_ENTITY_KEYEXPR:
            return zdcExecuteset_entity_keyexpr(pCmdPayload);

        case ZDC_CMD_SET_ENTITY_CONFIG:
            return zdcExecuteset_entity_config(pCmdPayload);

        default:
            // This case can be customized (e.g to send an error command)
            return false;
    }
}
