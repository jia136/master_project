#ifndef _HC_SR04_H_
#define _HC_SR04_H_

#include <driver/gpio.h>
#include <esp_err.h>

#define ESP_ERR_ULTRASONIC_PING         0x200
#define ESP_ERR_ULTRASONIC_PING_TIMEOUT 0x201
#define ESP_ERR_ULTRASONIC_ECHO_TIMEOUT 0x202

typedef struct {
    gpio_num_t trigger_pin; // GPIO output pin for trigger
    gpio_num_t echo_pin;    // GPIO input pin for echo
} ultrasonic_sensor_t;

esp_err_t ultrasonic_init(const ultrasonic_sensor_t *dev);

esp_err_t ultrasonic_measure_raw(const ultrasonic_sensor_t *dev, uint32_t max_time_us, uint32_t *time_us);

esp_err_t ultrasonic_measure(const ultrasonic_sensor_t *dev, float max_distance, float *distance);

#endif