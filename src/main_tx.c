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
//

#include "LCSF_Bridge_zdc_a.h"
#include "zdc_Main_a.h"
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <zenoh-pico.h>

#include "zenoh-pico/api/macros.h"
#include "zenoh-pico/api/types.h"

int main(int argc, char **argv) {
    int opt;
    char *keyexpr = NULL;
    char *value = NULL;
    char *cmd = NULL;
    int eid = -1;

    while ((opt = getopt(argc, argv, "k:v:i:c:")) != -1) {
        switch (opt) {
            case 'k':
                keyexpr = optarg;
                break;
            case 'v':
                value = optarg;
                break;
            case 'i':
                eid = atoi(optarg);
                break;
            case 'c':
                cmd = optarg;
                break;
            case '?':
                if (optopt == 'k' || optopt == 'v' || optopt == 'i' || optopt == 'c') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                }
                return 1;
            default:
                return -1;
        }
    }

    z_owned_config_t config;
    z_config_default(&config);

    printf("Opening session...\n");
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

    // Init lcsf
    LCSF_TranscoderInit();
    LCSF_ValidatorInit(NULL, NULL);
    zdc_MainInit();

    // Process command
    uint8_t *buffer = NULL;
    size_t buff_size = 0;

    if (strcmp(cmd, "keyexpr") == 0) {
        if (!zdc_encode_keyexpr(eid, value, &buffer, &buff_size)) {
            printf("Failed encoding keyexpr\n");
            return -1;
        }
    } else if (strcmp(cmd, "state") == 0) {
        int state = 0;
        if (strcmp(value, "on") == 0) {
            state = 1;
        }
        if (!zdc_encode_state(eid, (uint_fast8_t)state, &buffer, &buff_size)) {
            printf("Failed encoding state\n");
            return -1;
        }
    } else if (strcmp(cmd, "config") == 0) {
        printf("Not yet supported\n");
    }
    // Create payload
    if ((buffer == NULL) || (buff_size == 0)) {
        printf("Failed encoding command\n");
        return -1;
    }
    z_owned_bytes_t payload;
    z_bytes_serialize_from_slice(&payload, buffer, buff_size);
    z_view_keyexpr_t ke;
    z_view_keyexpr_from_str(&ke, keyexpr);

    // printf("Putting Data on '%s': ", keyexpr);
    // for (size_t i = 0; i < buff_size; i++) {
    //     printf("0x%02x, ", buffer[i]);
    // }
    // printf("\n");
    printf("Sending command\n");
    if (z_put(z_loan(s), z_loan(ke), z_move(payload), NULL) < 0) {
        printf("Oh no! Put has failed...\n");
    }
    // Clean up
    z_close(z_move(s));
    return 0;
}
