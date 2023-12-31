idf_component_register(SRCS "main.c" "../ESP_NET/wifi.c" "../ESP_MOTION/motion.c" "../ESP_LOG/esp_logging.c" "../ESP_LOG/esp_logging_buffer.c" "../ESP_WEB/socket.c"
                        INCLUDE_DIRS "." "../ESP_NET" "../ESP_MOTION" "../ESP_LOG" "../ESP_WEB"
                        PRIV_REQUIRES nvs_flash esp_http_client esp_websocket_client)
