#include <stdio.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_logging.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include "socket.h"

#define NO_DATA_TIMEOUT_SEC 5


static TimerHandle_t shutdown_signal_timer;
static SemaphoreHandle_t shutdown_sema;

static char log_level_from_server = 0;
static char cap_level_from_server = 0;

static void shutdown_signaler(TimerHandle_t xTimer)
{
    xSemaphoreGive(shutdown_sema);
}

#if CONFIG_WEBSOCKET_URI_FROM_STDIN
static void get_string(char *line, size_t size)
{
    int count = 0;
    while (count < size) {
        int c = fgetc(stdin);
        if (c == '\n') {
            line[count] = '\0';
            break;
        } else if (c > 0 && c < 127) {
            line[count] = c;
            ++count;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

#endif /* CONFIG_WEBSOCKET_URI_FROM_STDIN */

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        break;
    case WEBSOCKET_EVENT_DATA:
        if (data->op_code == 0x08 && data->data_len == 2) {
        } else {
            char * log_info = (char *)data->data_ptr;

            if (*(log_info) == 'L') {
                log_level_from_server = *(log_info + 1);
            }
            else if (*(log_info) == 'C') {
                cap_level_from_server = *(log_info + 1);
            }

        }        

        xTimerReset(shutdown_signal_timer, portMAX_DELAY);
        break;
    case WEBSOCKET_EVENT_ERROR:

        break;
    }
}

void websocket_app_start(const char * log_data, int16_t log_capacity)
{
    esp_websocket_client_config_t websocket_cfg = {};

    shutdown_signal_timer = xTimerCreate("Websocket shutdown timer", NO_DATA_TIMEOUT_SEC * 1000 / portTICK_PERIOD_MS,
                                         pdFALSE, NULL, shutdown_signaler);
    shutdown_sema = xSemaphoreCreateBinary();

#if CONFIG_WEBSOCKET_URI_FROM_STDIN
    char line[128];

    get_string(line, sizeof(line));

    websocket_cfg.uri = line;


#else
    websocket_cfg.uri = CONFIG_WEBSOCKET_URI;

#endif /* CONFIG_WEBSOCKET_URI_FROM_STDIN */



    esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);

    esp_websocket_client_start(client);
    xTimerStart(shutdown_signal_timer, portMAX_DELAY);
           
    vTaskDelay(1000 / portTICK_RATE_MS);
   
    if (esp_websocket_client_is_connected(client)) {

        int res = esp_websocket_client_send_text(client, log_data, log_capacity, portMAX_DELAY);

    }
    vTaskDelay(1000 / portTICK_RATE_MS);

    xSemaphoreTake(shutdown_sema, portMAX_DELAY);
    esp_websocket_client_close(client, portMAX_DELAY);
    esp_websocket_client_destroy(client);
    
    unsigned short ui16_log_level = log_level_from_server - '0';
    short i16_cap_level = cap_level_from_server - '0';

    log_level_set(ui16_log_level);
    log_capacity_set(i16_cap_level);

}
