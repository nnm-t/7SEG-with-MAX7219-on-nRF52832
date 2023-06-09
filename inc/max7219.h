#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>

#define MAX7219_NUM_CASCADING				1

#define MAX7219_REGISTER_NO_OPERATION		0x00
#define MAX7219_REGISTER_DIGIT_0			0x01
#define MAX7219_REGISTER_DIGIT_1			0x02
#define MAX7219_REGISTER_DIGIT_2			0x03
#define MAX7219_REGISTER_DIGIT_3			0x04
#define MAX7219_REGISTER_DIGIT_4			0x05
#define MAX7219_REGISTER_DIGIT_5			0x06
#define MAX7219_REGISTER_DIGIT_6			0x07
#define MAX7219_REGISTER_DIGIT_7			0x08
#define MAX7219_REGISTER_DECODE_MODE		0x09
#define MAX7219_REGISTER_INTENSITY			0x0A
#define MAX7219_REGISTER_SCAN_LIMIT			0x0B
#define MAX7219_REGISTER_SHUTDOWN			0x0C
#define MAX7219_REGISTER_DISPLAY_TEST		0x0F

#define MAX7219_DECODE_NO					0x00
#define MAX7219_DECODE_CODE_B_0				0x01
#define MAX7219_DECODE_CODE_B_0_TO_3		0x0F
#define MAX7219_DECODE_CODE_B_0_TO_7		0xFF

#define MAX7219_CODE_B_HYPHEN				0x0A
#define MAX7219_CODE_B_E					0x0B
#define MAX7219_CODE_B_H					0x0C
#define MAX7219_CODE_B_L					0x0D
#define MAX7219_CODE_B_P					0x0E
#define MAX7219_CODE_B_BLANK				0x0F
#define MAX7219_CODE_B_COMMA				0x80

#define MAX7219_INTENSITY_1_PER_32			0x00
#define MAX7219_INTENSITY_3_PER_32			0x01
#define MAX7219_INTENSITY_5_PER_32			0x02
#define MAX7219_INTENSITY_7_PER_32			0x03
#define MAX7219_INTENSITY_9_PER_32			0x04
#define MAX7219_INTENSITY_11_PER_32			0x05
#define MAX7219_INTENSITY_13_PER_32			0x06
#define MAX7219_INTENSITY_15_PER_32			0x07
#define MAX7219_INTENSITY_17_PER_32			0x08
#define MAX7219_INTENSITY_19_PER_32			0x09
#define MAX7219_INTENSITY_21_PER_32			0x0A
#define MAX7219_INTENSITY_23_PER_32			0x0B
#define MAX7219_INTENSITY_25_PER_32			0x0C
#define MAX7219_INTENSITY_27_PER_32			0x0D
#define MAX7219_INTENSITY_29_PER_32			0x0E
#define MAX7219_INTENSITY_31_PER_32			0x0F

#define MAX7219_SCAN_LIMIT_0				0x00
#define MAX7219_SCAN_LIMIT_0_TO_1			0x01
#define MAX7219_SCAN_LIMIT_0_TO_2			0x02
#define MAX7219_SCAN_LIMIT_0_TO_3			0x03
#define MAX7219_SCAN_LIMIT_0_TO_4			0x04
#define MAX7219_SCAN_LIMIT_0_TO_5			0x05
#define MAX7219_SCAN_LIMIT_0_TO_6			0x06
#define MAX7219_SCAN_LIMIT_0_TO_7			0x07

#define MAX7219_SHUTDOWN_EXECUTE			0x00
#define MAX7219_SHUTDOWN_CANCEL				0x01

#define MAX7219_DISPLAY_TEST_ON				0x01
#define MAX7219_DISPLAY_TEST_OFF			0x00

struct max7219_config {
	const struct device* device_spi;
	const struct gpio_dt_spec* gpio_load;
	struct spi_config* spi_config;
	uint32_t num_cascading;
};

struct max7219_data {
	uint8_t *tx_buf;
};

int max7219_init(struct max7219_config* config);

int max7219_write(struct max7219_config* config, struct max7219_data* data, const uint8_t address, const uint8_t value);

int max7219_decode_mode_no(struct max7219_config* config, struct max7219_data* data);

int max7219_decode_mode_b_0(struct max7219_config* config, struct max7219_data* data);

int max7219_decode_mode_b_0_to_3(struct max7219_config* config, struct max7219_data* data);

int max7219_decode_mode_b_0_to_7(struct max7219_config* config, struct max7219_data* data);

int max7219_set_intensity(struct max7219_config* config, struct max7219_data* data, const uint8_t value);

int max7219_set_scan_limit(struct max7219_config* config, struct max7219_data* data, const uint8_t value);

int max7219_shutdown_execute(struct max7219_config* config, struct max7219_data* data);

int max7219_shutdown_cancel(struct max7219_config* config, struct max7219_data* data);

int max7219_display_test_on(struct max7219_config* config, struct max7219_data* data);

int max7219_display_test_off(struct max7219_config* config, struct max7219_data* data);