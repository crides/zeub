#include <zephyr/device.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/drivers/led.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zeub_strip, CONFIG_ZEUB_LOG_LEVEL);

#include "strip.h"
#include <stdlib.h>

#define WHITE_BRIGHT 0
#define YELLOW_BRIGHT 20

#define MINUTE(h, m) (h * 60 + m)
static struct strip_cmd strip_cmds[CONFIG_STRIP_CMD_MAX_LEN] = {
    {7, 30, 0, 5},
    {7, 40, 0, 10},
    {7, 50, 1, 15},
    {8, 0, 2, 20},
    {10, 0, 5, 20},
    {12, 0, 7, 25},
    {15, 0, 5, 20},
    {17, 0, 2, 20},
    {18, 0, 1, 17},
    {19, 0, 0, 17},
    {20, 0, 0, 15},
    {21, 0, 0, 10},
    {22, 0, 0, 5},
    {23, 0, 0, 0},
};
static const struct device *led_strip = DEVICE_DT_GET(DT_NODELABEL(led_strip));
static uint8_t brights[4] = {WHITE_BRIGHT, YELLOW_BRIGHT, 0, 0};

#define STRIP_SVC_UUID(n) BT_UUID_128_ENCODE(0x9c990000 | n, 0x910f, 0x48f2, 0x8a70, 0xddfe5218a98a)

static struct bt_uuid_128 strip_uuid = BT_UUID_INIT_128(STRIP_SVC_UUID(0)),
                          strip_char_uuid = BT_UUID_INIT_128(STRIP_SVC_UUID(1));

static ssize_t read_strip_cmds(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset);
static ssize_t write_strip_cmds(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset, uint8_t flags);
static void strip_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value);

BT_GATT_SERVICE_DEFINE(strip_svc,
	BT_GATT_PRIMARY_SERVICE(&strip_uuid),
	BT_GATT_CHARACTERISTIC(&strip_char_uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       read_strip_cmds, write_strip_cmds, strip_cmds),
	BT_GATT_CCC(strip_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

static void proc_strip_cmds(struct k_work *work);
static K_WORK_DEFINE(proc_strip_cmd_work, proc_strip_cmds);

static ssize_t read_strip_cmds(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset) {
	return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data, sizeof(strip_cmds));
}

static ssize_t write_strip_cmds(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
	uint8_t *value = attr->user_data;

	if (offset + len > sizeof(strip_cmds)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}
	memcpy(value + offset, buf, len);
    /* k_work_submit(proc_strip_cmd_work); */
    return len;
}

static void strip_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value) {
    LOG_DBG("ccc cfg changed %04x", value);
}

static void proc_strip_cmds() {
    /* int cmds_len = 0; */
    /* while (true) { */
    /*     const struct strip_cmd cmd = strip_cmds[cmds_len]; */
    /*     if (cmd.min == 0 && cmd.white == 0 && cmd.yellow == 0) { */
    /*         return */
    /* q_sort( */
}

static bool cmd_empty(const struct strip_cmd *cmd) {
    return cmd->hour == 0 && cmd->min == 0 && cmd->white == 0 && cmd->yellow == 0;
}

static int valid_cmds_len() {
    for (int i = 0; i < CONFIG_STRIP_CMD_MAX_LEN; i ++) {
        if (cmd_empty(&strip_cmds[i])) {
            return i;
        }
    }
    return CONFIG_STRIP_CMD_MAX_LEN;
}

static int set_bright() {
    int ret;
    for (uint8_t i = 0; i < sizeof(brights); i ++) {
        ret = led_set_brightness(led_strip, i, brights[i]);
        if (ret < 0) {
            LOG_ERR("set strip %d: %d", i, ret);
            return ret;
        }
    }
    return 0;
}

static int cmp_hour_min(const uint8_t h, const uint8_t m, const uint8_t cmd_ind) {
    const struct strip_cmd cmd = strip_cmds[cmd_ind];
    if (h > cmd.hour) {
        return 1;
    } else if (h < cmd.hour) {
        return -1;
    } else {
        if (m > cmd.min) {
            return 1;
        } else if (m < cmd.min) {
            return -1;
        } else {
            return 0;
        }
    }
}

void strip_handle_time(const struct time_state *time) {
    static int found_cmd_ind = -1, last_found_ind = -1;
    const uint8_t hour_now = time->time.tm_hour, min_now = time->time.tm_min;
    int start = 0, valid_len = valid_cmds_len(), end = valid_len, ret = 0;
    LOG_DBG("valid len %d now %u %u", end, hour_now, min_now);
    while (start <= end) {
        const int mid = (start + end) / 2;
        const int time_cmp = cmp_hour_min(hour_now, min_now, mid);
        LOG_DBG("%d %d mid %u %u cmp %d", start, end, strip_cmds[mid].hour, strip_cmds[mid].min, time_cmp);
        if (time_cmp > 0) {
            ret = mid;
            start = mid + 1;
        } else if (time_cmp < 0) {
            end = mid - 1;
        } else {
            ret = mid;
            break;
        }
    }
    LOG_DBG("ret %d cmp %d", ret, cmp_hour_min(hour_now, min_now, ret));
    if (cmp_hour_min(hour_now, min_now, ret) >= 0) {
        found_cmd_ind = ret;
    } else {
        found_cmd_ind = valid_len - 1;
    }
    if (found_cmd_ind != last_found_ind) {
        const struct strip_cmd cmd = strip_cmds[found_cmd_ind];
        LOG_INF("new found cmd [%d] { %u:%u (%u %u) }", found_cmd_ind, cmd.hour, cmd.min, cmd.white, cmd.yellow);
        brights[0] = cmd.white;
        brights[1] = cmd.yellow;
        set_bright();
        last_found_ind = found_cmd_ind;
    }
}

void change_bright(const uint8_t strip, const int8_t change) {
    if (change < 0) {
        const uint8_t abs = (uint8_t) (-change);
        if (brights[strip] > abs) {
            brights[strip] -= abs;
        } else {
            brights[strip] = 0;
        }
    } else {
        if (brights[strip] + change > 100) {
            brights[strip] = 100;
        } else {
            brights[strip] += change;
        }
    }
    set_bright();
}

static int strip_init(const struct device *_arg) {
    int ret;
    brights[0] = WHITE_BRIGHT;
    brights[1] = YELLOW_BRIGHT;
    ret = set_bright();
    if (ret < 0) {
        LOG_ERR("init set bright %d", ret);
        return ret;
    }
    LOG_INF("cmds {");
    const int valid_len = valid_cmds_len();
    for (int i = 0; i < valid_len; i ++) {
        const struct strip_cmd cmd = strip_cmds[i];
        LOG_INF("  { %u:%u, (%u %u) }", cmd.hour, cmd.min, cmd.white, cmd.yellow);
    }
    LOG_INF("}");
    return 0;
}

SYS_INIT(strip_init, APPLICATION, CONFIG_ZEUB_UI_INIT_PRIORITY);
