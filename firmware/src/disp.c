#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/timeutil.h>
#include <zephyr/drivers/led_strip.h>
#include <lvgl.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zeub_disp, CONFIG_ZEUB_LOG_LEVEL);

#include "zeub_time.h"
#include "clock.c"

const char *months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char *weekdays[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

static const struct device *oled = DEVICE_DT_GET(DT_NODELABEL(oled)),
        *temp_sensor = DEVICE_DT_GET(DT_NODELABEL(tmp117)),
        *rgb_matrix = DEVICE_DT_GET(DT_NODELABEL(rgb_matrix)),
        *white_matrix = DEVICE_DT_GET(DT_NODELABEL(white_matrix));
struct disp_screen {
    lv_obj_t *screen,
             *time, *temp;
};

static struct led_rgb pixels[64];
static uint8_t whites[256];
static struct disp_screen screen;
LV_FONT_DECLARE(minecraft_font);
static const lv_font_t *dot_font = &minecraft_font;

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

static void update_ui(struct k_work *work);
static K_WORK_DEFINE(update_ui_work, update_ui);

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

static void matrix_draw_chars(const char *chars, const uint8_t len) {
    lv_font_glyph_dsc_t glyph;
    int x = 2;
    memset(whites, 0, 256);
    for (int i = 0; i < len; i ++) {
        const bool found = lv_font_get_glyph_dsc(dot_font, &glyph, chars[i], chars[i + 1]);
        if (!found) {
            LOG_ERR("can't find glyph for %02x", chars[i]);
            continue;
        }
        if (x + glyph.ofs_x + glyph.box_w >= 32) {
            LOG_WRN("can't finish drawing glyph #%d %x", i, chars[i]);
        }
        if (glyph.bpp != 1) {
            LOG_ERR("non monochrome font");
            return;
        }
        const int pos_x = x + glyph.ofs_x;
        const int pos_y = (dot_font->line_height - dot_font->base_line) - glyph.box_h - glyph.ofs_y;
        uint8_t *bitmap = lv_font_get_glyph_bitmap(dot_font, chars[i]);
        uint8_t mask = 0x80;
        for (int r = 0; r < glyph.box_h; r ++) {
            for (int c = 0; c < glyph.box_w; c++) {
                if ((c + pos_x) < 32) {
                    const uint8_t ind = (pos_y + r) * 32 + pos_x + c;
                    whites[ind] = (mask & *bitmap) ? 255 : 0;
                }
                mask >>= 1;
                if (mask == 0) {
                    bitmap ++;
                    mask = 0x80;
                }
            }
        }
        x += glyph.adv_w;
    }
    led_strip_update_channels(white_matrix, whites, 256);
}

static char time_buf[8];
void disp_update_time(const struct time_state *time, const bool direct) {
    const uint8_t disp_min = ((!direct && time->time.tm_sec > 30) ? 1 : 0) + time->time.tm_min;
    snprintf(time_buf, 6, "%02d:%02u", time->time.tm_hour, disp_min);
    lv_label_set_text_fmt(screen.time, "%s %02d %s", months[time->time.tm_mon], time->time.tm_mday, time_buf);
    k_work_submit_to_queue(&display_work_q, &update_ui_work);
}

static void update_ui(struct k_work *work) {
    matrix_draw_chars(time_buf, 5);
}

LV_FONT_DECLARE(iosevka_16);
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

    for (int r = 0; r < 8; r ++) {
        for (int c = 0; c < 8; c ++) {
            const int ind = r * 8 + c;
            const int pix_off = ind * 3;
            pixels[ind].r = IMG_CLOCK[pix_off];
            pixels[ind].g = IMG_CLOCK[pix_off + 1];
            pixels[ind].b = IMG_CLOCK[pix_off + 2];
        }
    }
    led_strip_update_rgb(rgb_matrix, pixels, 64);
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
