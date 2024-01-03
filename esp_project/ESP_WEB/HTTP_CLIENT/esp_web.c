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


esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
            break;
        default:
            break;
        }
    return ESP_OK;
}

void post_rest_function(float temp, float pres, int alarm_activation) {
    esp_http_client_config_t config_post = {
        .url = "http://192.168.1.7:3000/sensor",
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .event_handler = client_event_post_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_post);
    char buf[100];
    snprintf(buf, 100, "{\"temp\":\"%.2f\", \"pres\":\"%.2f\",\"alarm\":\"%d\"}", temp, pres, alarm_activation);
    char  *post_data = buf;
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}
