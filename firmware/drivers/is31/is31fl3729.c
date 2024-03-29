/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT issi_is31fl3729

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/i2c.h>
#include <drivers/led_strip.h>
#include <sys/math_extras.h>
#include <sys/util.h>

#include "is31fl3729.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(is31, CONFIG_ZEUB_LOG_LEVEL);

static int is31fl3729_reg_write(const struct device *dev, uint8_t addr, uint8_t value) {
    const struct is31fl3729_config *config = dev->config;
    int ret = i2c_reg_write_byte_dt(&config->i2c, addr, value);
    if (ret < 0) {
        LOG_ERR("Failed writing to reg %02x on address %02x", addr, config->i2c.addr);
        return -EIO;
    }
    return 0;
}

static int is31fl3729_reg_burst_write(const struct device *dev, uint8_t start_addr,
                                      const uint8_t *buffer, size_t num_bytes) {
    const struct is31fl3729_config *config = dev->config;
    int ret = i2c_burst_write_dt(&config->i2c, start_addr, buffer, num_bytes);
    if (ret < 0) {
        LOG_ERR("Failed burst writing from reg %02x to address %02x", start_addr, config->i2c.addr);
        return -EIO;
    }
    return 0;
}

static inline bool num_pixels_ok(const struct is31fl3729_config *config, size_t num_pixels) {
    size_t num_bytes;
    const bool overflow = size_mul_overflow(num_pixels, 3, &num_bytes);
    return !overflow && (num_bytes <= IS31FL3729_BUFFER_SIZE);
}

/**
 * Updates individual LED channels without an RGB interpretation.
 */
static int is31fl3729_strip_update_channels(const struct device *dev, uint8_t *channels, size_t num_channels) {
    if (IS31FL3729_BUFFER_SIZE < num_channels) {
        return -ENOMEM;
    }
    int ret = is31fl3729_reg_burst_write(dev, IS31FL3729_PWM_BASE, channels, num_channels);
    if (ret < 0) {
        LOG_ERR("update channels %d", ret);
        return -EIO;
    }
    return 0;
}

/*
 * Updates the RGB LED matrix according to devicetree's map property.
 */
static int is31fl3729_strip_update_rgb(const struct device *dev, struct led_rgb *pixels, size_t num_pixels) {
    const struct is31fl3729_config *config = dev->config;
    struct is31fl3729_data *data = dev->data;

    if (!num_pixels_ok(config, num_pixels)) {
        return -ENOMEM;
    }

    for (int i = 0; i < num_pixels; i ++) {
        data->px_buffer[config->rgb_map[3 * i]] = config->gamma[pixels[i].r];
        data->px_buffer[config->rgb_map[3 * i + 1]] = config->gamma[pixels[i].g];
        data->px_buffer[config->rgb_map[3 * i + 2]] = config->gamma[pixels[i].b];
    }
    return is31fl3729_strip_update_channels(dev, data->px_buffer, IS31FL3729_BUFFER_SIZE);
}

/*
 * Initiates a driver instance for IS31FL3729.
 *
 * SDB is pulled high to enable chip operation followed
 * by a reset to clear out all previous values.
 *
 * Function and scaling registers are then pre-configured based on devicetree settings.
 */
int static is31fl3729_init(const struct device *dev) {
    const struct is31fl3729_config *config = dev->config;

    int ret = gpio_pin_configure_dt(&config->sdb_gpio, GPIO_OUTPUT);
    if (ret < 0) {
        LOG_ERR("configure sdb %d", ret);
        return -EIO;
    }

    ret = gpio_pin_set_dt(&config->sdb_gpio, 1);
    if (ret < 0) {
        LOG_ERR("pull sdb high %d", ret);
        return -EIO;
    }

    // Reset
    ret = is31fl3729_reg_write(dev, IS31FL3729_RESET_REG, IS31FL3729_RESET_VAL); // Reset
    if (ret < 0) {
        LOG_ERR("reset %d", ret);
        return ret;
    }

    // SWS, H logic, Normal operation
    ret = is31fl3729_reg_write(dev, IS31FL3729_CONFIG_REG, (config->sws << 4) | (0x01 << 3) | 0x01);
    if (ret < 0) {
        LOG_ERR("conf %d", ret);
        return ret;
    }
    ret = is31fl3729_reg_write(dev, IS31FL3729_GCC_REG, config->gcc);
    if (ret < 0) {
        LOG_ERR("gcc %d", ret);
        return ret;
    }

    ret = is31fl3729_reg_burst_write(dev, IS31FL3729_SCALING_BASE, config->scaling, IS31FL3729_CS_NUM);
    if (ret < 0) {
        LOG_ERR("scaling %d", ret);
        return ret;
    }

    /* uint8_t open_short[18]; */
    /* ret = i2c_burst_read_dt(&config->i2c, IS31FL3729_OPEN_SHORT_BASE, open_short, 18); */
    /* if (ret < 0) { */
    /*     LOG_ERR("read %d", ret); */
    /* } */
    /* LOG_HEXDUMP_INF(open_short, 18, "open short"); */

    LOG_INF("is31 init on %02x", config->i2c.addr);

    return 0;
}

static const struct led_strip_driver_api is31fl3729_api = {
    .update_rgb = is31fl3729_strip_update_rgb,
    .update_channels = is31fl3729_strip_update_channels,
};

#define IS31FL3729_GCC(idx)                                                                        \
    (DT_INST_PROP(idx, r_ext) * DT_INST_PROP(idx, led_max_current) * 64 * 256) / (342 * 255)

#define IS31FL3729_DEVICE(idx)                                                                     \
    static uint8_t is31fl3729_##idx##_px_buffer[IS31FL3729_BUFFER_SIZE];                           \
                                                                                                   \
    static struct is31fl3729_data is31fl3729_##idx##_data = {                                      \
        .px_buffer = is31fl3729_##idx##_px_buffer,                                                 \
    };                                                                                             \
                                                                                                   \
    static uint8_t is31fl3729_##idx##_rgb_map[IS31FL3729_BUFFER_SIZE] = DT_INST_PROP(idx, map);    \
    static uint8_t is31fl3729_##idx##_gamma[256] = DT_INST_PROP(idx, gamma);                       \
    static uint8_t is31fl3729_##idx##_scaling[IS31FL3729_CS_NUM] = DT_INST_PROP(idx, scaling);     \
                                                                                                   \
    static const struct is31fl3729_config is31fl3729_##idx##_config = {                            \
        .i2c = I2C_DT_SPEC_GET(DT_DRV_INST(idx)),                                                  \
        .sdb_gpio = GPIO_DT_SPEC_GET(DT_DRV_INST(idx), sdb_gpios),                                 \
        .gcc = IS31FL3729_GCC(idx),                                                                \
        .sws = DT_INST_PROP(idx, sw_setting),                                                      \
        .rgb_map = is31fl3729_##idx##_rgb_map,                                                     \
        .gamma = is31fl3729_##idx##_gamma,                                                         \
        .scaling = is31fl3729_##idx##_scaling,                                                     \
    };                                                                                             \
                                                                                                   \
    DEVICE_DT_INST_DEFINE(idx, &is31fl3729_init, NULL, &is31fl3729_##idx##_data,                   \
                          &is31fl3729_##idx##_config, POST_KERNEL, CONFIG_LED_STRIP_INIT_PRIORITY, \
                          &is31fl3729_api);

DT_INST_FOREACH_STATUS_OKAY(IS31FL3729_DEVICE);
