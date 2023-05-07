#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

#include "max7219.h"

#define LED0_NODE				DT_ALIAS(led0)
#define SPI0_NODE				DT_NODELABEL(my_spi0)
#define SPI0_SS_NODE			DT_ALIAS(myspi0ss)

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec spi0_ss = GPIO_DT_SPEC_GET(SPI0_SS_NODE, gpios);

static struct spi_config spi_config = {
	.operation = SPI_WORD_SET(8),
	// 100kHz
	.frequency = 1000000
};

static struct max7219_config max7219_config = {
	.spi_config = &spi_config,
	.gpio_load = &spi0_ss,
	.num_cascading = MAX7219_NUM_CASCADING
};

static uint8_t max7219_tx_buf[MAX7219_NUM_CASCADING * 2];

static struct max7219_data max7219_data = {
	.tx_buf = max7219_tx_buf
};

void main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);

	// MAX7219
	// device 取得
	max7219_config.device_spi = DEVICE_DT_GET(SPI0_NODE);

	int ret = max7219_init(&max7219_config);
	if (ret < 0)
	{
		printk("MAX7219 init error: %d", ret);
		return;
	}

	k_msleep(100);
	// Display Test
	max7219_display_test_on(&max7219_config, &max7219_data);
	k_msleep(100);
	max7219_display_test_off(&max7219_config, &max7219_data);
	max7219_shutdown_cancel(&max7219_config, &max7219_data);
	// Decode Mode
	max7219_set_scan_limit(&max7219_config, &max7219_data, MAX7219_SCAN_LIMIT_0_TO_7);
	max7219_decode_mode_b_0_to_7(&max7219_config, &max7219_data);
	k_msleep(100);

	max7219_write(&max7219_config, &max7219_data, 0x01, 0x03);
	max7219_write(&max7219_config, &max7219_data, 0x02, 0x03);
	max7219_write(&max7219_config, &max7219_data, 0x03, 0x0F);
	max7219_write(&max7219_config, &max7219_data, 0x04, 0x04);

	max7219_write(&max7219_config, &max7219_data, 0x05, 0x0F);
	max7219_write(&max7219_config, &max7219_data, 0x06, 0x0F);
	max7219_write(&max7219_config, &max7219_data, 0x07, 0x0A);
	max7219_write(&max7219_config, &max7219_data, 0x08, 0x04);
	
	while (1) {
		k_msleep(50);
	}
}
