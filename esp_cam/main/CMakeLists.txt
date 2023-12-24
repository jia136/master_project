idf_component_register(SRCS "main.c" "../ESP_NET/wifi.c" "../ESP_MOTION/motion.c"
                        INCLUDE_DIRS "." "../ESP_NET" "../ESP_MOTION"
                        PRIV_REQUIRES nvs_flash esp_http_client)
