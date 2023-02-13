#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>

#define MAX7219_NUM_CASCADING				1

#define MAX7219_REGISTER_DISPLAY_TEST		0x0F

#define MAX7219_DISPLAY_TEST_ON				0x01
#define MAX7219_DISPLAY_TEST_OFF			0x00

struct max7219_config {
	const struct device* device_spi;
	struct spi_config* spi_config;
	uint32_t num_cascading;
};

struct max7219_data {
	uint8_t *tx_buf;
};

int max7219_init(struct max7219_config* config);

int max7219_write(struct max7219_config* config, struct max7219_data* data, const uint8_t address, const uint8_t value);