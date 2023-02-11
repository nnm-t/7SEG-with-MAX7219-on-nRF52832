#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE				DT_ALIAS(led0)

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0});

static int led1_value = GPIO_ACTIVE_LOW;

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
