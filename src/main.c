#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

#define LED0_NODE				DT_ALIAS(led0)
#define SPI0_NODE				DT_NODELABEL(my_spi0)

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0});

static int led1_value = GPIO_ACTIVE_LOW;

void main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);

	const struct device* device_spi0 = DEVICE_DT_GET(SPI0_NODE);
	struct spi_config spi_config = {0};
	spi_config.operation = SPI_WORD_SET(16);
	// 100kHz
	spi_config.frequency = 1000000;

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

	if (!device_is_ready(device_spi0))
	{
		printk("spi0 is not ready");
	}

	/*
	uint8_t address[1];
	uint8_t data[1];
	struct spi_buf bufs[] = {
		{
			.buf = address
		},
		{
			.buf = data,
			.len = sizeof(data)
		}
	};
	struct spi_buf_set tx_buf = {
		.buffers = bufs
	};
	ret = spi_write(device_spi0, &spi_config, &tx_buf);
	*/

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
