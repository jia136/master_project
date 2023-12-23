idf_component_register(SRCS "main.c" "../ESP_NET/wifi.c"
                        INCLUDE_DIRS "." "../ESP_NET"
                        PRIV_REQUIRES nvs_flash esp_http_client)
