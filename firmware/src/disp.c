#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/timeutil.h>
#include <lvgl.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zeub_disp, CONFIG_ZEUB_LOG_LEVEL);

#include "zeub_time.h"

const char *months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char *weekdays[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

static const struct device *oled = DEVICE_DT_GET(DT_NODELABEL(oled)),
             *temp_sensor = DEVICE_DT_GET(DT_NODELABEL(tmp117));
struct disp_screen {
    lv_obj_t *screen,
             *time, *temp;
};

static struct disp_screen screen;

static void display_tick_cb(struct k_work *work);
static void display_timer_cb();
static K_WORK_DEFINE(display_tick_work, display_tick_cb);
static K_THREAD_STACK_DEFINE(display_work_stack_area, CONFIG_ZEUB_DISPLAY_THREAD_STACK_SIZE);
static K_TIMER_DEFINE(display_timer, display_timer_cb, NULL);

static void temp_update_work_cb(struct k_work *work);
static void temp_update_timer_cb();
static K_WORK_DEFINE(temp_update_work, temp_update_work_cb);
static K_TIMER_DEFINE(temp_update_timer, temp_update_timer_cb, NULL);

static void init_ui(struct k_work *work);
static K_WORK_DEFINE(init_ui_work, init_ui);

static struct k_work_q display_work_q;

static void display_timer_cb() {
    k_work_submit_to_queue(&display_work_q, &display_tick_work);
}

static void display_tick_cb(struct k_work *work) { lv_task_handler(); }

static void temp_update_timer_cb() { k_work_submit_to_queue(&display_work_q, &temp_update_work); }

static void temp_update_work_cb(struct k_work *work) {
    int ret;
    ret = sensor_sample_fetch(temp_sensor);
    if (ret < 0) {
        LOG_ERR("temp fetch %d", ret);
        return;
    }
    struct sensor_value temp;
    ret = sensor_channel_get(temp_sensor, SENSOR_CHAN_AMBIENT_TEMP, &temp);
    if (ret < 0) {
        LOG_ERR("temp get %d", ret);
        return;
    }
    /* LOG_INF("temp: %d.%d C", temp.val1, temp.val2); */
    lv_label_set_text_fmt(screen.temp, "temp: %d.%02d C", temp.val1, temp.val2 / 10000);
}

void disp_update_time(const struct time_state *time, const bool direct) {
    const uint8_t disp_min = ((!direct && time->time.tm_sec > 30) ? 1 : 0) + time->time.tm_min;
    lv_label_set_text_fmt(screen.time, "%s %02d %02d:%02u", months[time->time.tm_mon - 1], time->time.tm_mday, time->time.tm_hour, disp_min);
}

LV_FONT_DECLARE(iosevka_16);
/* LV_FONT_DECLARE(iosevka_8); */
lv_style_t style;

static void init_ui(struct k_work *work) {
    screen.screen = lv_obj_create(NULL);
    if (screen.screen == NULL) {
        LOG_ERR("ui screen");
        return;
    }
    screen.time = lv_label_create(screen.screen);
    if (screen.time == NULL) {
        LOG_ERR("ui time");
        return;
    }
    screen.temp = lv_label_create(screen.screen);
    if (screen.temp == NULL) {
        LOG_ERR("ui temp");
        return;
    }
    lv_style_set_text_font(&style, &iosevka_16);
    /* lv_style_set_text_color(&style, lv_color_white()); */
    /* lv_style_set_bg_color(&style, lv_color_black()); */
    lv_obj_add_style(screen.screen, &style, LV_PART_MAIN);
    lv_obj_set_width(screen.time, lv_pct(100));
    lv_obj_align(screen.time, LV_ALIGN_TOP_MID, 0, 0);
    lv_label_set_text(screen.time, "");
    lv_obj_set_width(screen.temp, lv_pct(100));
    lv_obj_align(screen.temp, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_scr_load(screen.screen);
}

static int disp_init(const struct device *_arg) {
    display_blanking_off(oled);

    k_work_queue_init(&display_work_q);
    k_work_queue_start(&display_work_q, display_work_stack_area, K_THREAD_STACK_SIZEOF(display_work_stack_area),
                       CONFIG_ZEUB_DISPLAY_THREAD_PRIORITY, NULL);

    k_work_submit_to_queue(&display_work_q, &init_ui_work);
    k_work_submit_to_queue(&display_work_q, &temp_update_work);

    k_timer_start(&temp_update_timer, K_MSEC(CONFIG_TEMP_UPDATE_INTERVAL_MS), K_MSEC(CONFIG_TEMP_UPDATE_INTERVAL_MS));
    k_timer_start(&display_timer, K_MSEC(CONFIG_LV_DISP_DEF_REFR_PERIOD), K_MSEC(CONFIG_LV_DISP_DEF_REFR_PERIOD));
    return 0;
}

SYS_INIT(disp_init, APPLICATION, CONFIG_ZEUB_UI_INIT_PRIORITY);
