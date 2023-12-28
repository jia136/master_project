#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"
#include "driver/timer.h"
#include <esp_err.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "hc_sr04.h"
#include "buzzer.h"
#include "bmp280.h"
#include "wifi.h"
#include "esp_time.h"
#include "esp_web.h"
#include "socket.h"
#include "esp_logging.h"

#define MAX_DISTANCE_CM 500 // 5m max distance for ultrasonic sensor

//GPIO pins
//Ultrasonic sensor
#define TRIGGER_ULTRASONIC_GPIO 5
#define ECHO_ULTRASONIC_GPIO    19
//Buzzer-alarm
#define BUZZER_GPIO             18
//BMP280
#define BMP280_SCL_GPIO         22
#define BMP280_SDA_GPIO         21
//timer
#define TIMER_DIVIDER   (16)
#define DELAY_5_MIN     1500000000
#define DELAY_1_MIN      300000000

#define ESP_INTR_FLAG_DEFAULT 0

static xQueueHandle timer_evt_send_queue = NULL;


static bool IRAM_ATTR timer_group_isr_callback(void * arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(timer_evt_send_queue, &gpio_num, NULL);
    return true;
}

static int alarm_activation_num = 0;

void ultrasonic_task(void *pvParameters) {

    ultrasonic_sensor_t sensor = {
        .trigger_pin = TRIGGER_ULTRASONIC_GPIO,
        .echo_pin = ECHO_ULTRASONIC_GPIO
    };

    buzzer_t buzzer = {
        .buzzer_pin = BUZZER_GPIO
    };

    ultrasonic_init(&sensor);
    buzzer_init(&buzzer);

    bool last_state = 0; //0 = buzzer off, 1 = buzzer on
    
    while (true) {

        float distance;
        esp_err_t res = ultrasonic_measure(&sensor, MAX_DISTANCE_CM, &distance);
        if ( res != ESP_OK ) {
            printf("Error %d: ", res);
            switch (res) {
                case ESP_ERR_ULTRASONIC_PING:
                    printf("Cannot ping (device is in invalid state)\n");
                    break;
                case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                    printf("Ping timeout (no device found)\n");
                    break;
                case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                    printf("Echo timeout (i.e. distance too big)\n");
                    buzzer_set(&buzzer, BUZZER_OFF);
                    break;
                default:
                    printf("%s\n", esp_err_to_name(res));
            }
        }
        else {
            printf("Distance: %0.04f m \n", distance);
            if (distance < 0.3) {
                buzzer_set(&buzzer, BUZZER_ON);
                if ( !last_state ) {
                    ++alarm_activation_num;
                }
                last_state = 1;
            }
            else {
                buzzer_set(&buzzer, BUZZER_OFF);
                last_state = 0;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1500));

    }

}

void timer_task(void *pvParameters) {

    //timer config
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_AUTORELOAD_EN
    };
    //bmp280 sensor config
    i2c_config_t i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = BMP280_SDA_GPIO,
        .scl_io_num = BMP280_SCL_GPIO,
        .sda_pullup_en = false,
        .scl_pullup_en = false,

        .master = {
            .clk_speed = 100000
        }
    };
    //timer init
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, DELAY_1_MIN);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, timer_group_isr_callback, NULL, 0);
    timer_start(TIMER_GROUP_0, TIMER_0);

    //bmp280 init
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_cfg));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));

    bmx280_t* bmx280 = bmx280_create(I2C_NUM_0);

    if (!bmx280) { 
        ESP_LOGE("test", "Could not create bmx280 driver.");
        return;
    }

    ESP_ERROR_CHECK(bmx280_init(bmx280));

    bmx280_config_t bmx_cfg = BMX280_DEFAULT_CONFIG;
    ESP_ERROR_CHECK(bmx280_configure(bmx280, &bmx_cfg));
    uint32_t io_num;
    while (true) {

        if ( xQueueReceive(timer_evt_send_queue, &io_num, portMAX_DELAY) ) {
            
            ESP_ERROR_CHECK(bmx280_setMode(bmx280, BMX280_MODE_FORCE));
            do {
                vTaskDelay(pdMS_TO_TICKS(1));
            } while(bmx280_isSampling(bmx280));

            float temp = 0, pres = 0, hum = 0;
            ESP_ERROR_CHECK(bmx280_readoutFloat(bmx280, &temp, &pres, &hum));
            //bmp280 doesn't have hum sensor..
            ESP_LOGI("test", "Read Values: temp = %f, pres = %f", temp, pres);

            post_rest_function(temp, pres, alarm_activation_num);

            vTaskDelay(pdMS_TO_TICKS(100));
        }

    }
}

void app_main(void) {
   
    //create a queue to handle timer event from isr
    timer_evt_send_queue = xQueueCreate(10, sizeof(uint32_t));
    
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
    LOGE_0(3, 0X45);
    //create tasks
    //xTaskCreate(ultrasonic_task, "ultrasonic_task", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
    xTaskCreate(timer_task, "timer_task", configMINIMAL_STACK_SIZE * 3, NULL, 10, NULL);

}

