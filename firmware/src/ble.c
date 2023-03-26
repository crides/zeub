#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/byteorder.h>

#include "ble.h"
#include "zeub.h"
#include "zeub_time.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zeub_ble, CONFIG_ZEUB_LOG_LEVEL);

static struct bt_conn *time_server_conn = NULL;

#define ZEUB_BLE_PARAM BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE, \
        BT_GAP_ADV_SLOW_INT_MIN, BT_GAP_ADV_SLOW_INT_MAX, NULL)
static const struct bt_data zeub_ad_data[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, "Zeub", 4),
    BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE, BT_UUID_16_ENCODE(0x07C6)),   // Multi-color LED array
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    /* BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_BAS_VAL)), */
};

static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t err);
static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};

static uint8_t gatt_read(struct bt_conn *conn, uint8_t err, struct bt_gatt_read_params *params, const void *data, uint16_t length) {
    if (err) {
        if (err == BT_ATT_ERR_ATTRIBUTE_NOT_FOUND) {
            return BT_GATT_ITER_STOP;
        }
        LOG_ERR("read %u", err);
        return BT_GATT_ITER_STOP;
    }

    LOG_INF("read len %u", length);
    const uint8_t *buf = data;
    uint16_t year;
    memcpy(&year, buf, sizeof(uint16_t));
    year = sys_le16_to_cpu(year);
    struct time_state cur_time;
    cur_time.time.tm_year = year - 1900;
    cur_time.time.tm_mon = buf[2] - 1;
    cur_time.time.tm_mday = buf[3];
    cur_time.time.tm_hour = buf[4];
    cur_time.time.tm_min = buf[5];
    cur_time.time.tm_sec = buf[6];
    cur_time.time.tm_wday = buf[7] == 7 ? 0 : buf[7];
    cur_time.time.tm_isdst = buf[9];
    cur_time.frag = buf[8];
    time_set_cur(&cur_time);

    return BT_GATT_ITER_STOP;
}

static struct bt_gatt_read_params read_params = {
    .func = gatt_read,
    .handle_count = 0,
    .by_uuid = {
        .uuid = BT_UUID_CTS_CURRENT_TIME,
        .start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE,
        .end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE,
    },
};

int ble_read_cur_time() {
    int ret = bt_gatt_read(time_server_conn, &read_params);
    if (ret < 0) {
        LOG_ERR("read %d", ret);
        return ret;
    }
    return 0;
}

static void connected(struct bt_conn *conn, uint8_t err) {
    int ret;
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        LOG_WRN("connect %s %u", addr, err);
        return;
    }

    ret = bt_conn_set_security(conn, BT_SECURITY_L2);
    if (ret < 0) {
        LOG_ERR("sec %d", ret);
        return;
    }

    ret = bt_gatt_read(conn, &read_params);
    if (ret < 0) {
        LOG_ERR("read %d", ret);
    }

    time_server_conn = bt_conn_ref(conn);
}

static void disconnected(struct bt_conn *conn, uint8_t err) {
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        LOG_WRN("disconnect %s %u", addr, err);
        return;
    }

    bt_conn_unref(time_server_conn);
    time_server_conn = NULL;
}

static int ble_init(const struct device *_arg) {
    int ret;

    ret = bt_enable(NULL);
    if (ret < 0) {
        LOG_ERR("bt %d", ret);
        return ret;
    }

    ret = settings_subsys_init();
    if (ret < 0) {
        LOG_ERR("settings init %d", ret);
        return ret;
    }

    ret = settings_load_subtree("bt");

    if (ret < 0) {
        LOG_ERR("bt settings load %d", ret);
        return ret;
    }

    bt_conn_cb_register(&conn_callbacks);

    ret = bt_le_adv_start(ZEUB_BLE_PARAM, zeub_ad_data, ARRAY_SIZE(zeub_ad_data), NULL, 0);
    if (ret < 0) {
        LOG_ERR("adv %d", ret);
        return ret;
    }

    LOG_INF("zeub init");

    return 0;
}

SYS_INIT(ble_init, APPLICATION, CONFIG_BLE_INIT_PRIORITY);
