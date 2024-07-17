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

#include "zdc_Main_b.h"
#include "LCSF_Bridge_zdc_b.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zenoh-pico.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    // Initialize Zenoh Session and other parameters
    z_owned_config_t config;
    z_config_default(&config);

    // Open Zenoh session
    printf("Opening Zenoh Session...\n");
    z_owned_session_t s;
    if (z_open(&s, z_move(config)) < 0) {
        printf("Unable to open session!\n");
        return -1;
    }

    // Start read and lease tasks for zenoh-pico
    if (zp_start_read_task(z_loan_mut(s), NULL) < 0 || zp_start_lease_task(z_loan_mut(s), NULL) < 0) {
        printf("Unable to start read and lease tasks\n");
        z_close(z_session_move(&s));
        return -1;
    }

    // Init Lcsf
    LCSF_TranscoderInit();
    LCSF_ValidatorInit(NULL, NULL);
    if (!zdc_MainInit(z_loan(s))) {
        printf("Unable to init LCSF\n");
        exit(-1);
    } else {
        char *ke_suffix = zdc_entity_ke_suffix(0);
        printf("Listening to zdc commands on: %s\n", ke_suffix);
    }

    char buf[256];
    int idx = 0;
    while(true) {
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

    z_close(z_move(s));
}
