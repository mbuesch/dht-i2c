/*
 * DHT11/22 to I2C converter
 *
 * Copyright (c) 2018 Michael Buesch <m@bues.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "main.h"
#include "util.h"
#include "i2c_slave.h"
#include "dht.h"

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/crc16.h>

#include <string.h>


#define I2C_ADDR	0x76

static struct host_interface_state {
	uint8_t next_data[DHT_DATA_LEN + 2u];
	uint8_t tx_data[DHT_DATA_LEN + 2u];
	uint8_t tx_index;
} hostintf;


static void host_comm_start(void)
{
	/* Copy the message. */
	memcpy(hostintf.tx_data, hostintf.next_data, sizeof(hostintf.tx_data));

	/* Reset transmit byte index. */
	hostintf.tx_index = 0u;
}

static uint8_t host_transmit_callback(bool start)
{
	uint8_t data;

	if (start)
		host_comm_start();

	data = hostintf.tx_data[hostintf.tx_index];

	if (++hostintf.tx_index >= sizeof(hostintf.tx_data))
		hostintf.tx_index = 0u;

	return data;
}

static bool host_receive_callback(bool start, uint8_t byte)
{
	bool continue_transfer = false;

	host_comm_start();

	return continue_transfer;
}

static const struct i2c_slave_ops __flash host_interface_slave_ops = {
	.transmit	= host_transmit_callback,
	.receive	= host_receive_callback,
};

static void host_interface_init(void)
{
	memset(hostintf.next_data, 0u, sizeof(hostintf.next_data));
	memset(hostintf.tx_data, 0u, sizeof(hostintf.tx_data));
	hostintf.tx_index = 0u;

	i2cs_init();
	i2cs_add_slave(I2C_ADDR, &host_interface_slave_ops);
}

static void dht_rx_callback(const uint8_t *data)
{
	uint8_t i, crc;

	/* Increment message sequence counter. */
	hostintf.next_data[0]++;

	/* Copy the payload data. */
	memcpy(hostintf.next_data + 1u, data, DHT_DATA_LEN);

	/* Calculate CRC8 */
	crc = 0u;
	for (i = 0u; i < sizeof(hostintf.next_data) - 1u; i++)
		crc = _crc8_ccitt_update(crc, hostintf.next_data[i]);
	hostintf.next_data[sizeof(hostintf.next_data) - 1u] = crc;
}

static void basic_port_init(void)
{
	PORTB = 0u;
	DDRB = (0u << DDB0) | /* Input: ISP-MOSI / I2C-SDA */
	       (0u << DDB1) | /* Input: ISP-MISO */
	       (0u << DDB2) | /* Input: ISP-SCK / I2C-SCL */
	       (1u << DDB3) | /* Output: Debug pin */
	       (0u << DDB4) | /* Input: DHT signal */
	       (0u << DDB5);  /* Reset */
	PORTB = (0u << PB0) | /* Input, no pullup: ISP-MOSI / I2C-SDA */
		(1u << PB1) | /* Input, pullup: ISP-MISO */
		(0u << PB2) | /* Input, no pullup: ISP-SCK / I2C-SCL */
		(0u << PB3) | /* Output, low level: Debug pin */
		(1u << PB4) | /* Input, pullup: DHT signal */
		(0u << PB5);  /* Reset */
}

void early_init(void) __attribute__((naked, section(".init3"), used));
void early_init(void)
{
	MCUSR = 0;
	wdt_enable(WDTO_500MS);
}

int main(void) _mainfunc;
int main(void)
{
	basic_port_init();
	dht_init();
	host_interface_init();
	dht_start(dht_rx_callback);

	wdt_enable(WDTO_250MS);
	set_sleep_mode(SLEEP_MODE_IDLE);
	irq_enable();
	while (1) {
		sleep_mode();
		wdt_reset();
	}
}
