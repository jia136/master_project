#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <esp_err.h>
#include "sdkconfig.h"
#include "hc_sr04.h"
#include "buzzer.h"
#include "motion.h"
#include "wifi.h"
#include "esp_time.h"
#include "esp_web.h"

#define MAX_DISTANCE_CM 500 // 5m max distance for ultrasonic sensor

//GPIO pins
#define TRIGGER_ULTRASONIC_GPIO 5
#define ECHO_ULTRASONIC_GPIO    19
#define BUZZER_GPIO             18
#define MOTION_GPIO             4
#define MOTION_INPUT_PIN_SEL    (1ULL << MOTION_GPIO)
#define CAMERA_OUTPUT_GPIO      12
#define CAMERA_OUTPUT_PIN_SEL   (1ULL << CAMERA_OUTPUT_GPIO)

#define ESP_INTR_FLAG_DEFAULT 0

static xQueueHandle gpio_evt_motion_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg) {

    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_motion_queue, &gpio_num, NULL);

}

static void gpio_camera_task(void* arg) {

    uint32_t io_num;
    for(;;) {
        if ( xQueueReceive(gpio_evt_motion_queue, &io_num, portMAX_DELAY) ) {
            //POSTAVI NEKI PIN CAMERA_OUTPUT_GPIO NA VISOK NIVO
            //STAVI KAMERI DO ZNANJA DA FOTOGRAFISE
            printf("Kamero, ukljuci se\n");
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

}

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
            }
            else {
                buzzer_set(&buzzer, BUZZER_OFF);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}

/*void motion_task(void *pvParameters) {

    motion_t motion = {
        .motion_pin = MOTION_GPIO
    };
    motion_init(&motion);

    while (true) {

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}*/

void app_main(void) {
   
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set 
    io_conf.pin_bit_mask = CAMERA_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins
    io_conf.pin_bit_mask = MOTION_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //create a queue to handle gpio event from isr
    gpio_evt_motion_queue = xQueueCreate(10, sizeof(uint32_t));
    
    //create tasks
    //xTaskCreate(ultrasonic_task, "ultrasonic_task", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
    xTaskCreate(gpio_camera_task, "gpio_camera_task", configMINIMAL_STACK_SIZE * 3, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(MOTION_GPIO, gpio_isr_handler, (void*) MOTION_GPIO);

    //wifi module
    esp_err_t ret_wifi = nvs_flash_init();
    if (ret_wifi == ESP_ERR_NVS_NO_FREE_PAGES || ret_wifi == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret_wifi = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret_wifi);

    wifi_init();
    time_init();
    post_rest_function();
}
