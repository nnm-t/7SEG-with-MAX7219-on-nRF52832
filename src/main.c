#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

#include "max7219.h"

#define LED0_NODE				DT_ALIAS(led0)
#define SPI0_NODE				DT_NODELABEL(my_spi0)
#define SPI0_SS_NODE			DT_ALIAS(myspi0ss)

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec spi0_ss = GPIO_DT_SPEC_GET(SPI0_SS_NODE, gpios);

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

	if (!device_is_ready(spi0_ss.port))
	{
		printk("spi0_ss is not ready");
	}

	ret = gpio_pin_configure_dt(&spi0_ss, GPIO_OUTPUT_INACTIVE);
	if (ret < 0)
	{
		printk("gpio_pin_configure_dt() return code: %d", ret);
		return;
	}

	// MAX7219
	// device 取得
	max7219_config.device_spi = DEVICE_DT_GET(SPI0_NODE);

	ret = max7219_init(&max7219_config);
	if (ret < 0)
	{
		printk("MAX7219 init error: %d", ret);
		return;
	}

	k_msleep(100);

	// Display Test
	ret = max7219_display_test_on(&max7219_config, &max7219_data);
	if (ret < 0)
	{
		printk("MAX7219 display test error: %d", ret);
		return;
	}

	k_msleep(100);
	ret = gpio_pin_set_dt(&spi0_ss, GPIO_ACTIVE_HIGH);
	if (ret < 0)
	{
		printk("gpio_pin_set_dt() return code: %d", ret);
		return;
	}

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
