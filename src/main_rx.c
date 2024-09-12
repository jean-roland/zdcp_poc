//
// Copyright (c) 2022 ZettaScale Technology
//
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
// which is available at https://www.apache.org/licenses/LICENSE-2.0.
//
// SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
//
// Contributors:
//   ZettaScale Zenoh Team, <zenoh@zettascale.tech>

#include "LCSF_Bridge_zdc_b.h"
#include "zdc_Main_b.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zenoh-pico.h>

#define ZDC_DEF_CMD_KEYEXPR "sensor/1/zdcp"
#define ZDC_DEF_PUB_KEYEXPR "sensor/1/temp"
#define ZDC_DEF_SUB_KEYEXPR "sensor/1/data"
#define ZDC_BUFFER_SIZE 128
#define ZDC_ENTITY_NB 2

static void app_data_handler(const z_loaned_sample_t *sample, void *ctx) {
    (void)(ctx);
    z_view_string_t keystr;
    z_keyexpr_as_view_string(z_sample_keyexpr(sample), &keystr);
    z_owned_string_t value;
    z_bytes_deserialize_into_string(z_sample_payload(sample), &value);
    printf(">> [Subscriber] Received ('%s': '%s')\n", z_string_data(z_loan(keystr)), z_string_data(z_loan(value)));
    z_drop(z_move(value));
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    // Initialize Zenoh Session and other parameters
    z_owned_config_t config;
    z_config_default(&config);

    // Open Zenoh session
    printf("Opening Zenoh Session...\n");
    z_owned_session_t s;
    if (z_open(&s, z_move(config), NULL) < 0) {
        printf("Unable to open session!\n");
        return -1;
    }

    // Start read and lease tasks for zenoh-pico
    if (zp_start_read_task(z_loan_mut(s), NULL) < 0 || zp_start_lease_task(z_loan_mut(s), NULL) < 0) {
        printf("Unable to start read and lease tasks\n");
        z_close(z_session_move(&s), NULL);
        return -1;
    }

    // Init Lcsf
    LCSF_TranscoderInit();
    LCSF_ValidatorInit(NULL, NULL);
    if (!zdc_MainInit(z_loan(s), ZDC_ENTITY_NB, ZDC_BUFFER_SIZE, ZDC_DEF_CMD_KEYEXPR)) {
        printf("Unable to init LCSF\n");
        exit(-1);
    } else {
        char *ke_suffix = zdc_entity_ke_suffix(0);
        printf("Listening to zdc commands on: %s\n", ke_suffix);
    }
    // Add entities
    zdc_add_pub_entity(true, ZDC_DEF_PUB_KEYEXPR, NULL);
    zdc_add_sub_entity(false, ZDC_DEF_SUB_KEYEXPR, NULL, app_data_handler);

    char buf[256];
    int idx = 0;
    while (true) {
        sleep(1);
        if (zdc_entity_state(1) != 1) {
            idx = 0;
            continue;
        }
        // Create payload
        int value = z_random_u8() & 0x3f;
        sprintf(buf, "[%4d] %2dC", idx, value);
        z_owned_bytes_t payload;
        z_bytes_serialize_from_str(&payload, buf);

        // Retrieve keyexpr
        char *ke_suffix = zdc_entity_ke_suffix(1);
        z_view_keyexpr_t ke;
        z_view_keyexpr_from_str(&ke, ke_suffix);

        printf("Putting Data ('%s': '%s')...\n", ke_suffix, buf);
        z_put(z_loan(s), z_loan(ke), z_move(payload), NULL);
        idx++;
    }

    printf("Closing Zenoh Session...\n");

    // Stop the receive and the session lease loop for zenoh-pico
    zp_stop_read_task(z_loan_mut(s));
    zp_stop_lease_task(z_loan_mut(s));

    // Close lcsf
    zdc_close();

    z_close(z_move(s), NULL);
}
