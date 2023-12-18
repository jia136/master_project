#ifndef _MOTION_H_
#define _MOTION_H_
#include <driver/gpio.h>

typedef struct {
    gpio_num_t motion_pin; // GPIO input pin for motion sensor
} motion_t;

uint32_t motion_init(const motion_t *dev);

uint32_t motion_get(const motion_t *dev);

#endif

