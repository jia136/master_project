#ifndef _BUZZER_H_
#define _BUZZER_H_

#include <driver/gpio.h>
#include <esp_err.h>

#define BUZZER_OFF 1
#define BUZZER_ON  0

typedef struct {
    gpio_num_t buzzer_pin; // GPIO output pin for buzzer
} buzzer_t;

esp_err_t buzzer_init(const buzzer_t *dev);

esp_err_t buzzer_set(const buzzer_t *dev, uint32_t buzzer_active);

#endif
