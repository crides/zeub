#pragma once

#include <device.h>
#include <drivers/gpio.h>

#define IS31FL3729_BUFFER_SIZE 0x90
#define IS31FL3729_CS_NUM 16

#define IS31FL3729_PWM_BASE 0x01
#define IS31FL3729_SCALING_BASE 0x90
#define IS31FL3729_CONFIG_REG 0xA0
#define IS31FL3729_GCC_REG 0xA1
#define IS31FL3729_PULL_SEL_REG 0xB0
#define IS31FL3729_SPREAD_SPEC_REG 0xB1
#define IS31FL3729_PWM_FREQ_REG 0xB2
#define IS31FL3729_OPEN_SHORT_BASE 0xB3
#define IS31FL3729_RESET_REG 0xCF
#define IS31FL3729_RESET_VAL 0xAE

struct is31fl3729_config {
    const struct i2c_dt_spec i2c;
    const struct gpio_dt_spec sdb_gpio;
    uint8_t gcc;
    uint8_t sws;
    uint8_t *rgb_map;
    uint8_t *gamma;
    uint8_t *scaling;
};

struct is31fl3729_data {
    uint8_t *px_buffer;
};

