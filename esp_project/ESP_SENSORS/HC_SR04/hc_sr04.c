#include "hc_sr04.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include "esp32/rom/ets_sys.h"

#define TRIGGER_LOW_DELAY 4
#define TRIGGER_HIGH_DELAY 10
#define PING_TIMEOUT 6000
#define ROUNDTRIP_M 5800.0f

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#define PORT_ENTER_CRITICAL portENTER_CRITICAL(&mux)
#define PORT_EXIT_CRITICAL portEXIT_CRITICAL(&mux)
#define PORT_RETURN_CRITICAL(RES) do { PORT_EXIT_CRITICAL; return RES; } while(0)

#define timeout_expired(start, len) ((esp_timer_get_time() - (start)) >= (len))

esp_err_t ultrasonic_init(const ultrasonic_sensor_t *dev) {
  
    gpio_set_direction(dev->trigger_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(dev->echo_pin, GPIO_MODE_INPUT);

    return gpio_set_level(dev->trigger_pin, 0);
}

esp_err_t ultrasonic_measure_raw(const ultrasonic_sensor_t *dev, uint32_t max_time_us, uint32_t *time_us) {
    
    PORT_ENTER_CRITICAL;
    
    // Ping: Low for 2..4 us, then high 10 us
    gpio_set_level(dev->trigger_pin, 0);
    ets_delay_us(TRIGGER_LOW_DELAY);
    gpio_set_level(dev->trigger_pin, 1);
    ets_delay_us(TRIGGER_HIGH_DELAY);
    gpio_set_level(dev->trigger_pin, 0);

    // Previous ping isn't ended
    if (gpio_get_level(dev->echo_pin))
        PORT_RETURN_CRITICAL(ESP_ERR_ULTRASONIC_PING);

    // Wait for echo
    int64_t start = esp_timer_get_time();
    while (!gpio_get_level(dev->echo_pin)) {
        if (timeout_expired(start, PING_TIMEOUT))
            PORT_RETURN_CRITICAL(ESP_ERR_ULTRASONIC_PING_TIMEOUT);
    }

    // got echo, measuring
    int64_t echo_start = esp_timer_get_time();
    int64_t time = echo_start;
    while ( gpio_get_level(dev->echo_pin) ) {
        time = esp_timer_get_time();
        if ( timeout_expired(echo_start, max_time_us) )
            PORT_RETURN_CRITICAL(ESP_ERR_ULTRASONIC_ECHO_TIMEOUT);
    }

    PORT_EXIT_CRITICAL;

    *time_us = time - echo_start;

    return ESP_OK;
}

esp_err_t ultrasonic_measure(const ultrasonic_sensor_t *dev, float max_distance, float *distance) {

    uint32_t time_us;
    ultrasonic_measure_raw(dev, max_distance * ROUNDTRIP_M, &time_us);
    *distance = time_us / ROUNDTRIP_M;

    return ESP_OK;
}

