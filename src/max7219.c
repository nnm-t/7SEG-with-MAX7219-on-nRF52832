#include "max7219.h"

int max7219_init(struct max7219_config* config)
{
	if (!device_is_ready(config->device_spi))
	{
		printk("spi0 is not ready");
		return -ENODEV;
	}

	if (!device_is_ready(config->gpio_load->port))
	{
		printk("LOAD pin is not ready");
		return -ENODEV;
	}

	int ret = gpio_pin_configure_dt(config->gpio_load, GPIO_OUTPUT_INACTIVE);
	if (ret < 0)
	{
		printk("LOAD pin gpio_pin_configure_dt() return code: %d", ret);
		return ret;
	}

	return 0;
}

int max7219_write(struct max7219_config* config, struct max7219_data* data, const uint8_t address, const uint8_t value)
{
	struct spi_buf tx_buf = {
		.buf = data->tx_buf,
		.len = config->num_cascading * 2
	};
	struct spi_buf_set tx_buf_set = {
		.buffers = &tx_buf,
		.count = 1
	};

	for (uint32_t i = 0; i < config->num_cascading; i++)
	{
		data->tx_buf[i * 2] = address;
		data->tx_buf[i * 2 + 1] = value;
	}

	int ret = gpio_pin_set_dt(config->gpio_load, GPIO_ACTIVE_LOW);
	if (ret < 0)
	{
		return ret;
	}

	ret = spi_write(config->device_spi, config->spi_config, &tx_buf_set);
	if (ret < 0)
	{
		return ret;
	}

	return gpio_pin_set_dt(config->gpio_load, GPIO_ACTIVE_HIGH);
}

int max7219_decode_mode_no(struct max7219_config* config, struct max7219_data* data)
{
	return max7219_write(config, data, MAX7219_REGISTER_DECODE_MODE, MAX7219_DECODE_NO);
}

int max7219_decode_mode_b_0(struct max7219_config* config, struct max7219_data* data)
{
	return max7219_write(config, data, MAX7219_REGISTER_DECODE_MODE, MAX7219_DECODE_CODE_B_0);
}

int max7219_decode_mode_b_0_to_3(struct max7219_config* config, struct max7219_data* data)
{
	return max7219_write(config, data, MAX7219_REGISTER_DECODE_MODE, MAX7219_DECODE_CODE_B_0_TO_3);
}

int max7219_decode_mode_b_0_to_7(struct max7219_config* config, struct max7219_data* data)
{
	return max7219_write(config, data, MAX7219_REGISTER_DECODE_MODE, MAX7219_DECODE_CODE_B_0_TO_7);
}

int max7219_set_intensity(struct max7219_config* config, struct max7219_data* data, const uint8_t value)
{
	return max7219_write(config, data, MAX7219_REGISTER_INTENSITY, value);
}

int max7219_set_scan_limit(struct max7219_config* config, struct max7219_data* data, const uint8_t value)
{
	return max7219_write(config, data, MAX7219_REGISTER_SCAN_LIMIT, value);
}

int max7219_shutdown_execute(struct max7219_config* config, struct max7219_data* data)
{
	return max7219_write(config, data, MAX7219_REGISTER_SHUTDOWN, MAX7219_SHUTDOWN_EXECUTE);
}

int max7219_shutdown_cancel(struct max7219_config* config, struct max7219_data* data)
{
	return max7219_write(config, data, MAX7219_REGISTER_SHUTDOWN, MAX7219_SHUTDOWN_CANCEL);
}

int max7219_display_test_on(struct max7219_config* config, struct max7219_data* data)
{
	return max7219_write(config, data, MAX7219_REGISTER_DISPLAY_TEST, MAX7219_DISPLAY_TEST_ON);
}

int max7219_display_test_off(struct max7219_config* config, struct max7219_data* data)
{
	return max7219_write(config, data, MAX7219_REGISTER_DISPLAY_TEST, MAX7219_DISPLAY_TEST_OFF);
}