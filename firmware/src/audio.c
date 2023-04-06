#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_audio.h>
#include <zephyr/drivers/i2s.h>

LOG_MODULE_REGISTER(zeub_audio, 3);

#define SAMPLE_FREQUENCY    48000
#define SAMPLE_BIT_WIDTH    16
#define TIMEOUT             1000

#define PACKET_SIZE 192
#define PACKET_RATIO 64
#define BLOCK_SIZE (PACKET_SIZE * PACKET_RATIO)
K_MEM_SLAB_DEFINE_STATIC(mem_slab, BLOCK_SIZE, 8, 4);

const struct device *i2s_dev_tx = DEVICE_DT_GET(DT_NODELABEL(i2s_tx));
static bool started = false;

static void data_received(const struct device *dev, struct net_buf *buffer, size_t size) {
    static int packet_ind = 0;
    static uint8_t *buf;
    int ret;

    if (!buffer || !size) {
        /* This should never happen */
        return;
    }

    if (packet_ind == 0) {
        ret = k_mem_slab_alloc(&mem_slab, (void **)&buf, K_NO_WAIT);
        if (ret < 0) {
            LOG_ERR("can't alloc slab: %d", ret);
        }
        LOG_DBG("alloc: %p", (void *) buf);
    }
    memcpy(&buf[PACKET_SIZE * packet_ind], buffer->data, PACKET_SIZE);
    packet_ind ++;
    if (packet_ind == PACKET_RATIO) {
        packet_ind = 0;
        ret = i2s_write(i2s_dev_tx, buf, BLOCK_SIZE);
        LOG_DBG("wrote");
        if (ret < 0) {
            LOG_ERR("i2s write: %d", ret);
        }
        if (!started) {
            ret = i2s_trigger(i2s_dev_tx, I2S_DIR_TX, I2S_TRIGGER_START);
            if (ret < 0) {
                LOG_ERR("Failed to trigger TX: %d", ret);
            }
            started = true;
            LOG_DBG("started");
        }
    }

    if (size == usb_audio_get_in_frame_size(dev)) {
        ret = usb_audio_send(dev, buffer, size);
        if (ret) {
            net_buf_unref(buffer);
        }
    } else {
        net_buf_unref(buffer);
    }
}

static void feature_update(const struct device *dev,
			   const struct usb_audio_fu_evt *evt)
{
	LOG_DBG("Control selector %d for channel %d updated",
		evt->cs, evt->channel);
	switch (evt->cs) {
	case USB_AUDIO_FU_MUTE_CONTROL:
	default:
		break;
	}
}

static const struct usb_audio_ops ops = {
	.data_received_cb = data_received,
	.feature_update_cb = feature_update,
};
const struct device *hs_dev = DEVICE_DT_GET_ONE(usb_audio_hp);

void usb_status_cb(enum usb_dc_status_code status, const uint8_t *param) {
    int ret;
    switch (status) {
        case USB_DC_DISCONNECTED:
            ret = i2s_trigger(i2s_dev_tx, I2S_DIR_TX, I2S_TRIGGER_STOP);
            if (ret < 0) {
                LOG_ERR("Failed to stop: %d", ret);
                return;
            }
            ret = i2s_trigger(i2s_dev_tx, I2S_DIR_TX, I2S_TRIGGER_DRAIN);
            if (ret < 0) {
                LOG_ERR("Failed to drain: %d", ret);
                return;
            }
            started = false;
            return;
        default:
            return;
    }
}

static int audio_init(const struct device *_arg) {
	int ret;

	struct i2s_config config;
	config.word_size = SAMPLE_BIT_WIDTH;
	config.channels = 2;
	config.format = I2S_FMT_DATA_FORMAT_I2S;
	config.options = I2S_OPT_BIT_CLK_MASTER | I2S_OPT_FRAME_CLK_MASTER;
	config.frame_clk_freq = SAMPLE_FREQUENCY;
	config.mem_slab = &mem_slab;
	config.block_size = BLOCK_SIZE;
	config.timeout = TIMEOUT;

	ret = i2s_configure(i2s_dev_tx, I2S_DIR_TX, &config);
	if (ret < 0) {
		LOG_ERR("Failed to configure TX stream: %d", ret);
        return -EIO;
	}

	usb_audio_register(hs_dev, &ops);

	ret = usb_enable(usb_status_cb);
	if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return -EIO;
	}

	LOG_INF("USB enabled");
    return 0;
}

SYS_INIT(audio_init, APPLICATION, CONFIG_ZEUB_UI_INIT_PRIORITY);
