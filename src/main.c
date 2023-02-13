#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

#include "max7219.h"

#define LED0_NODE				DT_ALIAS(led0)
#define SPI0_NODE				DT_NODELABEL(my_spi0)

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

static int led1_value = GPIO_ACTIVE_LOW;

static struct spi_config spi_config = {
	.operation = SPI_WORD_SET(8),
	// 100kHz
	.frequency = 1000000
};

static struct max7219_config max7219_config = {
	.spi_config = &spi_config,
	.num_cascading = MAX7219_NUM_CASCADING
};

static uint8_t max7219_tx_buf[MAX7219_NUM_CASCADING * 2];

static struct max7219_data max7219_data = {
	.tx_buf = max7219_tx_buf
};

void main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);

	if (!device_is_ready(led0.port))
	{
		printk("led0 is not ready");
		return;
	}

	int ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
	if (ret < 0)
	{
		printk("gpio_pin_configure_dt() return code: %d", ret);
		return;
	}

	// MAX7219
	// device 取得
	max7219_config.device_spi = DEVICE_DT_GET(SPI0_NODE);

	max7219_init(&max7219_config);
	// Display Test
	max7219_write(&max7219_config, &max7219_data, MAX7219_REGISTER_DISPLAY_TEST, MAX7219_DISPLAY_TEST_ON);

	while (1)
	{
		led1_value = led1_value == GPIO_ACTIVE_HIGH ? GPIO_ACTIVE_LOW : GPIO_ACTIVE_HIGH;
		ret = gpio_pin_set_dt(&led0, !led1_value);

		if (ret < 0)
		{
			printk("gpio_pin_set_dt() return code: %d", ret);
			return;
		}

		k_msleep(100);
	}
}
