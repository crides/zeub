#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/kscan.h>
#include <zephyr/drivers/sensor.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zeub, CONFIG_ZEUB_LOG_LEVEL);

#include "disp.h"
#include "zeub_time.h"
#include "strip.h"

static const struct device *buttons = DEVICE_DT_GET(DT_NODELABEL(buttons)),
             *roller = DEVICE_DT_GET(DT_NODELABEL(roller));

static void minute_timer_sync_work_cb(struct k_work *work);
static void minute_timer_cb();
static K_WORK_DELAYABLE_DEFINE(minute_timer_sync_work, minute_timer_sync_work_cb);
static K_TIMER_DEFINE(minute_timer, minute_timer_cb, NULL);

enum button {
    ENC_PUSH = 0,
    SEL_UP,
    SEL_DOWN,
    SEL_LEFT,
    SEL_RIGHT,
    SEL_PUSH,
    ROLL_UP,
    ROLL_DOWN,
};

static void handle_button(const struct device *dev, enum button button, bool pressed) {
    static uint8_t channel = 1;
    int ret;
    if (pressed) {
        switch (button) {
        case SEL_PUSH:
            break;
        case ENC_PUSH:
            channel = !channel;
            break;
        case ROLL_UP:
            change_bright(2 + channel, 1);
            break;
        case ROLL_DOWN:
            change_bright(2 + channel, -1);
            break;
        default:
            break;
        }
    }
}

static inline void buttons_cb(const struct device *dev, uint32_t row, uint32_t col, bool pressed) {
    LOG_DBG("buttons %u %d", col, pressed);
    handle_button(dev, col, pressed);
}

static inline void roller_cb(const struct device *dev, uint32_t row, uint32_t col, bool pressed) {
    LOG_DBG("roller %u %d", col, pressed);
    handle_button(dev, col + ROLL_UP, pressed);
}

void zeub_handle_time(const struct time_state *time) {
    const uint32_t rem_ms = (60 - time->time.tm_sec) * 1000 - time->frag * 1000 / 256;
    LOG_WRN("rem ms %u", rem_ms);
    disp_update_time(time, true);
    k_work_schedule(&minute_timer_sync_work, K_MSEC(rem_ms));
}

static void minute_timer_sync_work_cb(struct k_work *work) {
    int32_t skew_ms = k_timer_remaining_get(&minute_timer);
    if (skew_ms > 30000) {
        skew_ms = skew_ms - 60000;
    }
    LOG_INF("skew ms %d", skew_ms);
    k_timer_start(&minute_timer, K_NO_WAIT, K_MINUTES(1));
}

static void minute_timer_cb() {
    struct time_state now;
    int ret = time_get_unix_time(&now);
    if (ret < 0) {
        LOG_ERR("get time %d", ret);
        return;
    }
    disp_update_time(&now, false);
    strip_handle_time(&now);
}

int main() {
    int ret;

    kscan_config(roller, roller_cb);
    kscan_enable_callback(roller);
    kscan_config(buttons, buttons_cb);
    kscan_enable_callback(buttons);

    return 0;
}
