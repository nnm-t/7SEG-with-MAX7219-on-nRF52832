#include "max7219.h"

int max7219_init(struct max7219_config* config)
{
	if (!device_is_ready(config->device_spi))
	{
		printk("spi0 is not ready");
		return -ENODEV;
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

	return spi_write(config->device_spi, config->spi_config, &tx_buf_set);
}