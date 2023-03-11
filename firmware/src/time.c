#include <zephyr/device.h>
#include <zephyr/kernel.h>
/* #include <settings/settings.h> */

#include "zeub_time.h"
#include "ble.h"
#include "zeub.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zeub_time, CONFIG_ZEUB_LOG_LEVEL);

volatile bool time_synced = false;

static const char *months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static const char *weekdays[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const struct timeutil_sync_config time_sync_cfg = {
	.ref_Hz = 256,
	.local_Hz = 1000,
};

static struct timeutil_sync_state time_sync_state = {
    .cfg = &time_sync_cfg,
};

static void time_update_work_cb(struct k_work *work);
static void time_update_timer_cb();
static K_WORK_DEFINE(time_update_work, time_update_work_cb);
static K_TIMER_DEFINE(time_update_timer, time_update_timer_cb, NULL);

/* static int time_handle_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg); */
/* SETTINGS_STATIC_HANDLER_DEFINE(profiles_handler, "time", NULL, time_handle_set, NULL, NULL); */

uint64_t time_state_timegm(const struct time_state *time) {
    return timeutil_timegm64(&time->time) << 8 | time->frag;
}

uint32_t time_state_diff_ms(const struct time_state *a, const struct time_state *b) {
    return (time_state_timegm(a) - time_state_timegm(b)) * 1000 / 256;
}

void log_time_state(const char *msg, const struct time_state *time) {
    LOG_DBG("%s: %u %s %u, %s, %u:%u:%u.%03u", msg, time->time.tm_year + 1900, months[time->time.tm_mon], time->time.tm_mday, weekdays[time->time.tm_wday], time->time.tm_hour, time->time.tm_min, time->time.tm_sec, time->frag);
}

static void time_update_work_cb(struct k_work *work) {
    int ret = ble_read_cur_time();
    if (ret < 0) {
        LOG_ERR("ble read time %d", ret);
        return;
    }
}

void time_set_cur(const struct time_state *time) {
    log_time_state("set cur", time);
    const int64_t time_frac = time_state_timegm(time);
    const uint64_t uptime = k_uptime_get();
    const struct timeutil_sync_instant instant = {
        .ref = (uint64_t) time_frac,
        .local = uptime,
    };
    int ret = timeutil_sync_state_update(&time_sync_state, &instant);
    if (ret < 0) {
        LOG_WRN("time sync instant %d", ret);
        timeutil_sync_state_set_skew(&time_sync_state, 1.0, &instant);
    }
    if (ret == 1) {         // new instant is set
        const float est_skew = timeutil_sync_estimate_skew(&time_sync_state);
        LOG_INF("est skew %f %d ppb", est_skew, timeutil_sync_skew_to_ppb(est_skew));
        timeutil_sync_state_set_skew(&time_sync_state, est_skew, NULL);
    }

    zeub_handle_time(time);
    /* ret = settings_save_one("time/time", &time_sync_state, sizeof(time_sync_state)); */
    /* if (ret != 0) { */
    /*     LOG_ERR("save time %d", ret); */
    /* } */
}

static void time_update_timer_cb() {
    k_work_submit(&time_update_work);
}

static int time_init(const struct device *_arg) {
    k_timer_start(&time_update_timer, K_HOURS(CONFIG_TIME_UPDATE_INTERVAL_H), K_HOURS(CONFIG_TIME_UPDATE_INTERVAL_H));

    /* int ret = settings_load_subtree("time"); */
    /* if (ret < 0) { */
    /*     LOG_ERR("time init load %d", ret); */
    /*     return ret; */
    /* } */
    return 0;
}

/* static int time_handle_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) { */
/*     const char *next; */
/*     if (settings_name_steq(name, "time", &next) && !next) { */
/*         LOG_WRN("load time/time"); */
/*         int ret = read_cb(cb_arg, &time_sync_state, sizeof(time_sync_state)); */
/*         time_sync_state.cfg = &time_sync_cfg; */
/*         if (ret <= 0) { */
/*             LOG_ERR("time set %d", ret); */
/*             return ret; */
/*         } */
/*         time_synced = true; */
/*         struct time_state cur_time; */
/*         time_get_unix_time(&cur_time); */
/*         zeub_handle_time(&cur_time); */
/*     } */
/*     return 0; */
/* } */

int time_get_unix_time(struct time_state *time) {
    uint64_t ref_time;
    int ret = timeutil_sync_ref_from_local(&time_sync_state, k_uptime_get(), &ref_time);
    if (ret < 0) {
        LOG_ERR("sync time %d", ret);
        return ret;
    }
    const uint64_t unix_time = ref_time / time_sync_cfg.ref_Hz;
    memcpy(&time->time, gmtime(&unix_time), sizeof(struct tm));
    time->frag = unix_time % time_sync_cfg.ref_Hz;
    log_time_state("recover time", time);
    return 0;
}

SYS_INIT(time_init, APPLICATION, CONFIG_TIME_INIT_PRIORITY);
