#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_audio.h>
#include <zephyr/drivers/i2s.h>

LOG_MODULE_REGISTER(zeub_audio, CONFIG_ZEUB_LOG_LEVEL);

#define SAMPLE_FREQUENCY    44100
#define SAMPLE_BIT_WIDTH    16
#define BYTES_PER_SAMPLE    sizeof(int16_t)
#define NUMBER_OF_CHANNELS  2
/* Such block length provides an echo with the delay of 100 ms. */
#define SAMPLES_PER_BLOCK   ((SAMPLE_FREQUENCY / 10) * NUMBER_OF_CHANNELS)
#define INITIAL_BLOCKS      2
#define TIMEOUT             1000

#define BLOCK_SIZE 192
K_MEM_SLAB_DEFINE_STATIC(mem_slab, BLOCK_SIZE, 16, 4);

const struct device *i2s_dev_tx = DEVICE_DT_GET(DT_NODELABEL(i2s_tx));

static void data_received(const struct device *dev,
			  struct net_buf *buffer,
			  size_t size)
{
	int ret;

	if (!buffer || !size) {
		/* This should never happen */
		return;
	}

    /* size_t total_len = net_buf_frags_len(buffer); */
    uint8_t *buf;
	ret = k_mem_slab_alloc(&mem_slab, (void **) &buf, K_NO_WAIT);
    size_t recv = net_buf_linearize(buf, size, buffer, 0, size);
	LOG_DBG("Received %d data, buffer %p, recv %d", size, buffer, recv);
    ret = i2s_write(i2s_dev_tx, buf, BLOCK_SIZE);
    if (ret < 0) {
        LOG_ERR("i2s write: %d", ret);
    }
	ret = i2s_trigger(i2s_dev_tx, I2S_DIR_TX, I2S_TRIGGER_START);
	if (ret < 0) {
		LOG_ERR("Failed to trigger TX: %d", ret);
	}

    net_buf_unref(buffer);
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

static int audio_init(const struct device *_arg) {
	int ret;

	struct i2s_config config;
	config.word_size = SAMPLE_BIT_WIDTH;
	config.channels = NUMBER_OF_CHANNELS;
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

    uint8_t *buf;
    for (int i = 0; i < 4; i ++) {
        ret = k_mem_slab_alloc(&mem_slab, (void **) &buf, K_NO_WAIT);
        ret = i2s_write(i2s_dev_tx, buf, BLOCK_SIZE);
    }

	usb_audio_register(hs_dev, &ops);

	ret = usb_enable(NULL);
	if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return -EIO;
	}

	LOG_INF("USB enabled");
}

SYS_INIT(audio_init, APPLICATION, CONFIG_ZEUB_UI_INIT_PRIORITY);
