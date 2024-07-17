# App binary target
add_executable(${APP_TX_BIN_NAME} main_tx.c LCSF_Desc_zdc.c LCSF_Bridge_zdc_a.c zdc_Main_a.c)
# Dependencies
target_link_libraries(${APP_TX_BIN_NAME} PRIVATE lcsf_stack_lib zenohpico)

# App binary target
add_executable(${APP_RX_BIN_NAME} main_rx.c LCSF_Desc_zdc.c LCSF_Bridge_zdc_b.c zdc_Main_b.c)
# # Dependencies
target_link_libraries(${APP_RX_BIN_NAME} PRIVATE lcsf_stack_lib zenohpico)
