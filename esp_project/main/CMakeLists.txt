idf_component_register(SRCS "main.c" "../ESP_SENSORS/BUZZER/buzzer.c" "../ESP_SENSORS/HC_SR04/hc_sr04.c" "../ESP_SENSORS/MOTION/motion.c" "../ESP_NET/wifi.c" "../ESP_TIME/esp_time.c" "../ESP_WEB/esp_web.c"
                    INCLUDE_DIRS "." "../ESP_SENSORS/BUZZER" "../ESP_SENSORS/HC_SR04" "../ESP_SENSORS/MOTION" "../ESP_NET" "../ESP_TIME" "../ESP_WEB")
