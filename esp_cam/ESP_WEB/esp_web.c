#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "esp_web.h"
#include "esp_logging.h"

static char log_level_from_server = 0;
static char cap_level_from_server = 0;

esp_err_t from_server_post_handler(esp_http_client_event_handle_t evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
            char * log_info = (char *)evt->data;
            if (*(log_info) == 'L') {
                log_level_from_server = *(log_info + 1);
                unsigned short ui16_log_level = log_level_from_server - '0';  
                log_level_set(ui16_log_level);
            }
            if (*(log_info + 2) == 'C') {
                cap_level_from_server = *(log_info + 3);
                short i16_cap_level = cap_level_from_server - '0';
                log_capacity_set(i16_cap_level);
            }
            break;
        default:
            break;
        }
    return ESP_OK;
}

void send_log_function(const char * log_data, int16_t log_capacity) {
    esp_http_client_config_t config_post = {
        .url = "http://192.168.1.7:3000/logEspCam",
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .event_handler = from_server_post_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_post);
    esp_http_client_set_post_field(client, log_data, log_capacity);
    esp_http_client_set_header(client, "Content-Type", "text/plain");

    esp_http_client_perform(client);
    esp_http_client_cleanup(client);

}
