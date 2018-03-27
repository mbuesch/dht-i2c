/*
 * DHT11/22 driver
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

#include "dht.h"
#include "util.h"

#include <string.h>
#include <avr/interrupt.h>


#define DHT_PORT	PORTB
#define DHT_PIN		PINB
#define DHT_DDR		DDRB
#define DHT_BIT		4u


enum dht_state {
	DHT_WAIT,		/* Wait for the next data transfer */
	DHT_START_OUT_LOW,	/* Start: Output low */
	DHT_START_OUT_HIGH,	/* Start: Output high */
	DHT_START_IN_LOW,	/* Start: Input low */
	DHT_START_IN_HIGH,	/* Start: Input high */
	DHT_DATA_IN_LOW,	/* Data bit input: Low level */
	DHT_DATA_IN_HIGH,	/* Data bit input: High level */
};

struct dht {
	enum dht_state state;
	dht_callback_t callback;
	uint8_t timer_count;
	uint8_t byte_count;
	uint8_t bit_mask;
	uint8_t data[DHT_DATA_LEN];
	bool finished;
};


static struct dht dht;


static uint8_t dht_checksum(const uint8_t *data)
{
	uint8_t i, checksum = 0u;

	for (i = 0u; i < DHT_DATA_LEN - 1u; i++)
		checksum = ((uint8_t)(checksum + data[i]) & 0xFFu);

	return checksum;
}

static void dht_pin_change_irq_en(bool enable)
{
	if (enable) {
		GIFR = 1u << PCIF;
		GIMSK |= 1u << PCIE;
	} else {
		GIMSK &= (uint8_t)~(1u << PCIE);
	}
}

static void dht_pin_output(bool output, bool state)
{
	if (output) {
		DHT_DDR |= 1u << DHT_BIT;
		if (state)
			DHT_PORT |= 1u << DHT_BIT;
		else
			DHT_PORT &= (uint8_t)~(1u << DHT_BIT);

	} else {
		DHT_DDR &= (uint8_t)~(1u << DHT_BIT);
		DHT_PORT |= 1u << DHT_BIT;
	}
}

static void dht_pin_init(void)
{
	dht_pin_change_irq_en(false);
	PCMSK |= 1u << PCINT4;

	dht_pin_output(false, false);
}

static void dht_timer_init(void)
{
	TIMSK &= (uint8_t)~((1u << OCIE1A) | (1u << OCIE1B) | (1u << TOIE1));
	TIFR = (1u << OCF1A) | (1u << OCF1B) | (1u << TOV1);

	TCCR1 = 0u;
	TCNT1 = 0u;
	OCR1A = 0u;
	OCR1B = 0u;
	OCR1C = 0u;
	GTCCR &= (uint8_t)~((1u << PWM1B) |
			    (1u << COM1B1) | (1u << COM1B0) |
			    (1u << FOC1B) | (1u << FOC1A) |
			    (1u << PSR1));
	TCCR1 = (1u << CTC1) | (0u << PWM1A) |
		(0u << COM1A1) | (0u << COM1A0) |
		(0u << CS13) | (0u << CS12) | (0u << CS11) | (0u << CS10);
}

static void dht_timer_arm(void)
{
	uint8_t cs = 0u;
	uint8_t ocr = 0u;

	switch (dht.state) {
	case DHT_WAIT:
		/* Idle wait time. */

		/* Prescaler = 16384 */
		cs = (1u << CS13) | (1u << CS12) | (1u << CS11) | (1u << CS10);
		ocr = 98u;
		/* Wait duration = 100 ms * 13 = 1.3 s*/
		dht.timer_count = 13u;
		dht_pin_change_irq_en(false);
		break;
	case DHT_START_OUT_LOW:
		/* The start-out low period is about to start. */

		/* Prescaler = 16 */
		cs = (1u << CS13) | (1u << CS12) | (1u << CS11) | (1u << CS10);
		/* Output low duration = 20 ms */
		ocr = 20u;
		break;
	case DHT_START_OUT_HIGH:
	case DHT_START_IN_LOW:
	case DHT_START_IN_HIGH:
	case DHT_DATA_IN_LOW:
	case DHT_DATA_IN_HIGH:
		/* One of the periods is about to start. */

		/* Prescaler = 16 */
		cs = (0u << CS13) | (1u << CS12) | (0u << CS11) | (1u << CS10);
		/* Timeout = 150 us */
		ocr = 150u;

		if (dht.state == DHT_START_OUT_HIGH) {
			/* Enable pin change interrupt */
			dht_pin_change_irq_en(true);
		}
		break;
	}

	/* Re-program the timer. */
	TCCR1 &= (uint8_t)~((1u << CS13) | (1u << CS12) | (1u << CS11) | (1u << CS10));
	OCR1A = ocr;
	OCR1C = ocr;
	TCNT1 = 0u;
	TIFR = 1u << OCF1A;
	TIMSK |= 1u << OCIE1A;
	TCCR1 |= cs;
}

static void dht_timeout(void)
{
	/* Configure signal pin as input with pullup level = high */
	dht_pin_output(false, false);

	/* Go to initial state */
	dht.state = DHT_WAIT;
	dht_timer_arm();
}

/* Timer interrupt.
 * This interrupt is triggered on timing events or timeouts.
 */
ISR(TIMER1_COMPA_vect)
{
	memory_barrier();

	switch (dht.state) {
	case DHT_WAIT:
		/* Idle wait time. */
		if (--dht.timer_count == 0u) {
			/* End of wait time. */

			/* Configure signal pin as output with level = low */
			dht_pin_output(true, false);

			dht.state = DHT_START_OUT_LOW;
			dht_timer_arm();
		}
		break;
	case DHT_START_OUT_LOW:
		/* The start-out low period just ended. */

		/* Configure signal pin as input with pullup level = high */
		dht_pin_output(false, false);

		dht.state = DHT_START_OUT_HIGH;
		dht_timer_arm();
		break;
	case DHT_START_OUT_HIGH:
	case DHT_START_IN_LOW:
	case DHT_START_IN_HIGH:
	case DHT_DATA_IN_LOW:
	case DHT_DATA_IN_HIGH:
	default:
		/* Timeout waiting for input signal.
		 * The pin change interrupt did not trigger. */
		dht_timeout();
		break;
	}

	memory_barrier();
}

/* Pin change interrupt.
 * This interrupt is triggered on signal pin changes.
 */
ISR(PCINT0_vect)
{
	uint8_t timer, calc_checksum, data_checksum;
	bool pin_state;

	memory_barrier();

	/* First get the signal timer and signal pin states. */
	timer = TCNT1;
	pin_state = !!(DHT_PIN & (1u << DHT_BIT));

	switch (dht.state) {
	default:
	case DHT_WAIT:
	case DHT_START_OUT_LOW:
		/* Error: Invalid state machine state. */
		dht_timeout();
		break;
	case DHT_START_OUT_HIGH:
		/* The start-out high period just ended. */
		if (pin_state) {
			/* Error: Invalid pin state. */
			dht_timeout();
		} else {
			dht.state = DHT_START_IN_LOW;
			dht_timer_arm();
		}
		break;
	case DHT_START_IN_LOW:
		/* The start-in low period just ended. */
		if (pin_state) {
			dht.state = DHT_START_IN_HIGH;
			dht_timer_arm();
		} else {
			/* Error: Invalid pin state. */
			dht_timeout();
		}
		break;
	case DHT_START_IN_HIGH:
		/* The start-in high period just ended. */
		if (pin_state) {
			/* Error: Invalid pin state. */
			dht_timeout();
		} else {
			/* Prepare state for the first bit. */
			dht.byte_count = 0u;
			dht.bit_mask = 0x80u;
			dht.finished = false;

			dht.state = DHT_DATA_IN_LOW;
			dht_timer_arm();
		}
		break;
	case DHT_DATA_IN_LOW:
		/* The data-in low period just ended. */
		if (pin_state) {
			if (dht.finished) {
				dht.state = DHT_WAIT;

				/* We are done. Calculate and compare the checksum.
				 * If it matches, call the user callback. */
				calc_checksum = dht_checksum(dht.data);
				data_checksum = dht.data[DHT_DATA_LEN - 1u];
				if (calc_checksum == data_checksum)
					dht.callback(dht.data);
			} else
				dht.state = DHT_DATA_IN_HIGH;
			dht_timer_arm();
		} else {
			/* Error: Invalid pin state. */
			dht_timeout();
		}
		break;
	case DHT_DATA_IN_HIGH:
		/* The data-in high period just ended. */
		if (pin_state) {
			/* Error: Invalid pin state. */
			dht_timeout();
		} else {
			/* If the period was a long one, we have a 1 bit */
			if (timer >= 48u) {
				dht.data[dht.byte_count] |= dht.bit_mask;
			} else {
				dht.data[dht.byte_count] &= (uint8_t)~dht.bit_mask;
			}

			/* Shift mask to next bit. */
			dht.bit_mask >>= 1;
			if (dht.bit_mask == 0u) {
				dht.bit_mask = 0x80u;
				if (++dht.byte_count >= sizeof(dht.data)) {
					dht.byte_count = 0u;
					dht.finished = true;
				}
			}

			/* State = next bit */
			dht.state = DHT_DATA_IN_LOW;
			dht_timer_arm();
		}
		break;
	}

	memory_barrier();
}

void dht_start(dht_callback_t callback)
{
	if (dht.state == DHT_WAIT) {
		dht.callback = callback;
		memory_barrier();
		dht_timer_arm();
	}
}

void dht_init(void)
{
	memset(&dht, 0, sizeof(dht));
	dht.state = DHT_WAIT;

	dht_pin_init();
	dht_timer_init();
}
