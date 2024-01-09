#define BOARD_ESP32CAM_AITHINKER

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>
#include <esp_err.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "freertos/portmacro.h"

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif


#include <stdio.h>
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "sdkconfig.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "wifi.h"

#include "esp_logging.h"
#include "esp_log.h"
#include "esp_time.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "motion.h"


// WROVER-KIT PIN Map
#ifdef BOARD_WROVER_KIT

#define CAM_PIN_PWDN -1  //power down is not used
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 21
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 19
#define CAM_PIN_D2 18
#define CAM_PIN_D1 5
#define CAM_PIN_D0 4
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif

// ESP32Cam (AiThinker) PIN Map
#ifdef BOARD_ESP32CAM_AITHINKER

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif
#define MODULE_TAG 0 //esp_cam module

#define MOTION_GPIO             13
#define MOTION_INPUT_PIN_SEL    (1ULL << MOTION_GPIO)

#define ESP_INTR_FLAG_DEFAULT 0

#if ESP_CAMERA_SUPPORTED
static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_RGB565, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_QVGA,    //QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

    .jpeg_quality = 12, //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 1,       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

static xQueueHandle gpio_evt_motion_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg) {

    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_motion_queue, &gpio_num, NULL);

}

static esp_err_t init_camera(void) {
    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        LOGE_0(MODULE_TAG, 0x01);
        return err;
    }

    return ESP_OK;
}

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

void post_rest_function() {
    esp_http_client_config_t config_post = {
        .url = "http://192.168.1.7:3000/test",
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .event_handler = client_event_post_handler};
        
    camera_fb_t * fb = NULL;
    size_t fb_len = 0;
    uint8_t * fb_buf;
    fb = esp_camera_fb_get();

    if (!fb) {
        LOGE_0(MODULE_TAG, 0x02);
    }
    else {
        if  (fb->format == PIXFORMAT_JPEG) {
            LOGV_0(MODULE_TAG, 0x04);
            esp_http_client_handle_t client = esp_http_client_init(&config_post);
            esp_http_client_set_header(client, "Content-Type", "image/jpeg");
            esp_http_client_set_header(client, "Content-Length", (const char *)fb->buf);
            esp_http_client_set_header(client, "Content-Disposition", "inline; filename=capture.jpg");
            esp_http_client_set_post_field(client, (const char *)fb->buf, fb->len);

            esp_http_client_perform(client);
            esp_http_client_cleanup(client);
        } 
        else {
             LOGV_0(MODULE_TAG, 0x05);
             bool jpeg_converted = frame2jpg(fb, 80, &fb_buf, &fb_len);
             if(!jpeg_converted){
                LOGE_0(MODULE_TAG, 0x03);
                esp_camera_fb_return(fb);
            }
            else {
                LOGV_0(MODULE_TAG, 0x06);
                esp_http_client_handle_t client = esp_http_client_init(&config_post);
                esp_http_client_set_header(client, "Content-Type", "image/jpeg");
                esp_http_client_set_header(client, "Content-Length", (const char *)fb_buf);
                esp_http_client_set_header(client, "Content-Disposition", "inline; filename=capture.jpg");
                esp_http_client_set_post_field(client, (const char *)fb_buf, fb_len);

                esp_http_client_perform(client);
                esp_http_client_cleanup(client);
            }
            
        }

    }
    if(fb->format != PIXFORMAT_JPEG){
        free(fb_buf);
    }
    esp_camera_fb_return(fb);
}

#endif

static void gpio_camera_task(void* arg) {

    uint32_t io_num;
    for(;;) {
        if ( xQueueReceive(gpio_evt_motion_queue, &io_num, portMAX_DELAY) ) {
            post_rest_function();
            LOGI_0(MODULE_TAG, 0x07);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

}

void app_main(void) {
    //wifi module
    esp_err_t ret_wifi = nvs_flash_init();
    if (ret_wifi == ESP_ERR_NVS_NO_FREE_PAGES || ret_wifi == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret_wifi = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret_wifi);

    log_init();
    wifi_init();
    time_init();


#if ESP_CAMERA_SUPPORTED
    LOGV_0(MODULE_TAG, 0x0a);
    if(ESP_OK != init_camera()) {
        return;
    }
    LOGD_0(MODULE_TAG, 0x09);
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins
    io_conf.pin_bit_mask = MOTION_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    //create a queue to handle gpio event from isr
    gpio_evt_motion_queue = xQueueCreate(10, sizeof(uint32_t));

    //create task
    xTaskCreate(gpio_camera_task, "gpio_camera_task", configMINIMAL_STACK_SIZE * 3, NULL, 10, NULL);
    //install gpio isr service
    //gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(MOTION_GPIO, gpio_isr_handler, (void*) MOTION_GPIO);

    
#else
    LOGE_0(MODULE_TAG, 0x08);
    return;
#endif

}
