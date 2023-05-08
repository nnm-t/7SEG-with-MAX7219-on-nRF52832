#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>

#include "max7219.h"

#define LED0_NODE				DT_ALIAS(led0)
#define LED2_NODE				DT_ALIAS(led2)
#define LED3_NODE				DT_ALIAS(led3)
#define SPI0_NODE				DT_NODELABEL(my_spi0)
#define SPI0_SS_NODE			DT_ALIAS(myspi0ss)

#define BT_UUID_SERVICE			BT_UUID_128_ENCODE(0x1b36fd1e, 0x6fc4, 0x4dc6, 0x8c82, 0xc13552b88789)
#define BT_UUID_CHARACTERISTIC	BT_UUID_128_ENCODE(0x4ee58129, 0xa22f, 0x41cd, 0xbdca, 0x4079fe0632d0)
#define BT_CHARACTERISTIC_MAX_LENGTH		6

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);
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

static struct bt_uuid_128 service_uuid = BT_UUID_INIT_128(BT_UUID_SERVICE);
static struct bt_uuid_128 characteristic_uuid = BT_UUID_INIT_128(BT_UUID_CHARACTERISTIC);

static struct bt_data advertising_data[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_SERVICE)
};

static uint8_t characteristic_value[BT_CHARACTERISTIC_MAX_LENGTH] = { 0 } ;

void update_7seg()
{
	int32_t price = (characteristic_value[2] << 24) + (characteristic_value[3] << 16) + (characteristic_value[4] << 8) + characteristic_value[5];

	uint8_t digit_value[4] = { 0 };
	
	if (price < 0)
	{
		// マイナス
		// "----" 表示
		for (int i = 0; i < 4; i++)
		{
			digit_value[i] = 0x0A;
		}
	}
	else
	{
		digit_value[0] = (price / 1000) % 10;
		digit_value[1] = (price / 100) % 10;
		digit_value[2] = (price / 10) % 10;
		digit_value[3] = price % 10;

		// 1000 to 10 の位: 0のとき非表示
		for (int i = 0; i < 3; i++)
		{
			if (digit_value[i] == 0)
			{
				digit_value[i] = 0x0F;
			}
			else
			{
				break;
			}
		}
	}

	int i;

	switch (characteristic_value[0])
	{
		case 0:
			// 金額カウント
			// LED: 緑
			gpio_pin_set_dt(&led2, GPIO_ACTIVE_LOW);
			gpio_pin_set_dt(&led3, GPIO_ACTIVE_HIGH);
			// 緑: 金額表示
			for (i = 0x00; i < 0x04; i++)
			{
				max7219_write(&max7219_config, &max7219_data, i + 1, digit_value[i]);
			}

			for (i = 0x04; i < 0x08; i++)
			{
				max7219_write(&max7219_config, &max7219_data, i + 1, 0x0F);
			}
			break;
		case 1:
			// 金額入力
			// LED: 消灯
			gpio_pin_set_dt(&led2, GPIO_ACTIVE_LOW);
			gpio_pin_set_dt(&led3, GPIO_ACTIVE_LOW);
			// オレンジ: 金額表示
			for (i = 0x00; i < 0x04; i++)
			{
				max7219_write(&max7219_config, &max7219_data, i + 1, digit_value[i]);
			}

			for (i = 0x04; i < 0x08; i++)
			{
				max7219_write(&max7219_config, &max7219_data, i + 1, digit_value[i - 4]);
			}
			break;
		case 2:
			// 会計前 総額
			// LED: 赤
			gpio_pin_set_dt(&led2, GPIO_ACTIVE_HIGH);
			gpio_pin_set_dt(&led3, GPIO_ACTIVE_LOW);
			// 赤: 金額表示
			for (i = 0x00; i < 0x04; i++)
			{
				max7219_write(&max7219_config, &max7219_data, i + 1, 0x0F);
			}

			for (i = 0x04; i < 0x08; i++)
			{
				max7219_write(&max7219_config, &max7219_data, i + 1, digit_value[i - 4]);
			}
			break;
		case 3:
			// 会計完了
			// LED: 消灯
			gpio_pin_set_dt(&led2, GPIO_ACTIVE_LOW);
			gpio_pin_set_dt(&led3, GPIO_ACTIVE_LOW);
			// 緑: "0" へ戻す
			for (i = 0x00; i < 0x08; i++)
			{
				max7219_write(&max7219_config, &max7219_data, i + 1, 0x0F);
			}

			max7219_write(&max7219_config, &max7219_data, 0x04, 0x00);
			break;
		default:
			// その他
			// オレンジ: "----" 表示
			for (i = 0x00; i < 0x08; i++)
			{
				max7219_write(&max7219_config, &max7219_data, i + 1, 0x0A);
			}
			break;
	}
}

ssize_t on_read_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset)
{
	uint8_t* value = (uint8_t*)(attr->user_data);

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(characteristic_value));
}

static ssize_t on_write_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, const void* buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	uint8_t* value = (uint8_t*)(attr->user_data);

	if (offset + len > sizeof(characteristic_value))
	{
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value + offset, buf, len);

	update_7seg();

	return len;
}

BT_GATT_SERVICE_DEFINE(gatt_service,
	BT_GATT_PRIMARY_SERVICE(&service_uuid),
	BT_GATT_CHARACTERISTIC(&characteristic_uuid.uuid,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
		on_read_characteristic, on_write_characteristic, characteristic_value),
	BT_GATT_CUD("Type, Numbers, Price (6 bytes)", BT_GATT_PERM_READ)
);

static void on_mtu_updated(struct bt_conn* conn, uint16_t tx, uint16_t rx)
{
	printk("Updated MTU: TX=%d, RX=%d\n", tx, rx);
}

static struct bt_gatt_cb gatt_callbacks = {
	.att_mtu_updated = on_mtu_updated
};

static void on_connected(struct bt_conn* conn, uint8_t err)
{
	if (err)
	{
		printk("Connection failed: 0x%02x\n", err);
		return;
	}

	printk("Connected\n");
	// Advertising LED: 消灯
	gpio_pin_set_dt(&led0, GPIO_ACTIVE_LOW);
}

static void on_disconnected(struct bt_conn* conn, uint8_t reason)
{
	printk("Disconnected: 0x%02x\n", reason);
	// Advertising LED: 点灯
	gpio_pin_set_dt(&led0, GPIO_ACTIVE_HIGH);
}

BT_CONN_CB_DEFINE(connected_callbacks) = {
	.connected = on_connected,
	.disconnected = on_disconnected
};

void main(void)
{
	int32_t ret;

	// Advertising LED
	if (!device_is_ready(led0.port))
	{
		printk("LED0 GPIO is not ready\n");
		return;
	}

	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		printk("LED0 GPIO configure error: %d\n", ret);
		return;
	}

	// Red LED
	if (!device_is_ready(led2.port))
	{
		printk("LED2 GPIO is not ready\n");
		return;
	}

	ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		printk("LED2 GPIO configure error: %d\n", ret);
		return;
	}

	// Green LED
	if (!device_is_ready(led3.port))
	{
		printk("LED3 GPIO is not ready\n");
		return;
	}

	ret = gpio_pin_configure_dt(&led3, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		printk("LED3 GPIO configure error: %d\n", ret);
		return;
	}

	// LED: 点灯
	gpio_pin_set_dt(&led0, GPIO_ACTIVE_HIGH);

	// LED: 消灯
	gpio_pin_set_dt(&led2, GPIO_ACTIVE_LOW);
	gpio_pin_set_dt(&led3, GPIO_ACTIVE_LOW);

	// MAX7219
	// device 取得
	max7219_config.device_spi = DEVICE_DT_GET(SPI0_NODE);

	ret = max7219_init(&max7219_config);
	if (ret < 0)
	{
		printk("MAX7219 init error: %d\n", ret);
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

	// 緑: "0" と表示
	for (int i = 0x01; i <= 0x08; i++)
	{
		max7219_write(&max7219_config, &max7219_data, i, 0x0F);
	}
	max7219_write(&max7219_config, &max7219_data, 0x04, 0x00);

	// Bluetooth
	ret = bt_enable(NULL);

	if (ret)
	{
		printk("Bluetooth init failed: %d\n", ret);
		return;
	}

	if (IS_ENABLED(CONFIG_SETTINGS))
	{
		settings_load();
	}

	ret = bt_le_adv_start(BT_LE_ADV_CONN_NAME, advertising_data, ARRAY_SIZE(advertising_data), NULL, 0);
	if (ret)
	{
		printk("Advertising failed to start: %d\n", ret);
		return;
	}

	bt_gatt_cb_register(&gatt_callbacks);
	
	while (1) {
		k_msleep(50);
	}
}
