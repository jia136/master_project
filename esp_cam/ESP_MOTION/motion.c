#include "motion.h"

uint32_t motion_init(const motion_t *dev) {

    gpio_set_direction(dev->motion_pin, GPIO_MODE_INPUT);

    return gpio_get_level(dev->motion_pin);
}

uint32_t motion_get(const motion_t *dev) {

    return gpio_get_level(dev->motion_pin);

}