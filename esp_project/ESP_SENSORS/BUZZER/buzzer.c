#include "buzzer.h"

esp_err_t buzzer_init(const buzzer_t *dev) {

    gpio_set_direction(dev->buzzer_pin, GPIO_MODE_OUTPUT);

    return gpio_set_level(dev->buzzer_pin, BUZZER_OFF);
}

esp_err_t buzzer_set(const buzzer_t *dev, uint32_t buzzer_active) {

    return gpio_set_level(dev->buzzer_pin, buzzer_active);

}