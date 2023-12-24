/**
 * This example takes a picture every 5s and print its size on serial monitor.
 */

// =============================== SETUP ======================================

// 1. Board setup (Uncomment):
// #define BOARD_WROVER_KIT
#define BOARD_ESP32CAM_AITHINKER

/**
 * 2. Kconfig setup
 *
 * If you have a Kconfig file, copy the content from
 *  https://github.com/espressif/esp32-camera/blob/master/Kconfig into it.
 * In case you haven't, copy and paste this Kconfig file inside the src directory.
 * This Kconfig file has definitions that allows more control over the camera and
 * how it will be initialized.
 */

/**
 * 3. Enable PSRAM on sdkconfig:
 *
 * CONFIG_ESP32_SPIRAM_SUPPORT=y
 *
 * More info on
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html#config-esp32-spiram-support
 */

// ================================ CODE ======================================

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#include "esp_camera.h"
#include "esp_timer.h"

#include <stdio.h>
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "wifi.h"

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

static const char *TAG = "example:take_picture";

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

static esp_err_t init_camera(void) {
    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
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
        ESP_LOGE(TAG, "Camera capture failed");
    }
    else {
        if  (fb->format == PIXFORMAT_JPEG) {
            esp_http_client_handle_t client = esp_http_client_init(&config_post);
            esp_http_client_set_header(client, "Content-Type", "image/jpeg");
            esp_http_client_set_header(client, "Content-Length", (const char *)fb->buf);
            esp_http_client_set_header(client, "Content-Disposition", "inline; filename=capture.jpg");
            esp_http_client_set_post_field(client, (const char *)fb->buf, fb->len);

            esp_http_client_perform(client);
            esp_http_client_cleanup(client);
        } 
        else {
             ESP_LOGI(TAG, "chank ....");
             bool jpeg_converted = frame2jpg(fb, 80, &fb_buf, &fb_len);
             if(!jpeg_converted){
                ESP_LOGE(TAG, "JPEG compression failed");
                esp_camera_fb_return(fb);
            }
            else {
                esp_http_client_handle_t client = esp_http_client_init(&config_post);
                esp_http_client_set_header(client, "Content-Type", "image/jpeg");
                esp_http_client_set_header(client, "Content-Length", (const char *)fb_buf);
                esp_http_client_set_header(client, "Content-Disposition", "inline; filename=capture.jpg");
                esp_http_client_set_post_field(client, (const char *)fb_buf, fb_len);

                esp_http_client_perform(client);
                esp_http_client_cleanup(client);
                ESP_LOGI(TAG, "xo xo xo");
            }
            
        }

    }
    if(fb->format != PIXFORMAT_JPEG){
        free(fb_buf);
    }
    esp_camera_fb_return(fb);
}

#endif

void app_main(void) {
    //wifi module
    esp_err_t ret_wifi = nvs_flash_init();
    if (ret_wifi == ESP_ERR_NVS_NO_FREE_PAGES || ret_wifi == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret_wifi = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret_wifi);

    wifi_init();

#if ESP_CAMERA_SUPPORTED
    if(ESP_OK != init_camera()) {
        return;
    }

    post_rest_function();

#else
    ESP_LOGE(TAG, "Camera support is not available for this chip");
    return;
#endif


}