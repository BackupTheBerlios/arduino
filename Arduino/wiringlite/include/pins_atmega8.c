/*
pins_atmega8.h

David A. Mellis
21 April 2005
*/

#include <avr/io.h>
#include "BConstants.h"
#include "wiring.h"

// We map the pin numbers passed to digitalRead() or
// analogRead() directly to the corresponding pin 
// numbers on the Atmega8.  No distinction is made
// between analog and digital pins.

// ATMEL ATMEGA8
//
//       +-\/-+
// PC6  1|    |28  PC5
// PD0  2|    |27  PC4	
// PD1  3|    |26  PC3
// PD2  4|    |25  PC2
// PD3  5|    |24  PC1
// PD4  6|    |23  PC0
// VCC  7|    |22  GND
// GND  8|    |21  AREF
// PB6  9|    |20  AVCC
// PB7 10|    |19  PB5
// PD5 11|    |18  PB4
// PD6 12|    |17  PB3
// PD7 13|    |16  PB2
// PB0 14|    |15  PB1
//       +----+

#define NUM_PINS 28
#define NUM_PORTS 4

#define PB 2
#define PC 3
#define PD 4

int port_to_mode[NUM_PORTS + 1] = {
	NOT_A_PORT,
	NOT_A_PORT,
	_SFR_IO_ADDR(DDRB),
	_SFR_IO_ADDR(DDRC),
	_SFR_IO_ADDR(DDRD),
};

int port_to_output[NUM_PORTS + 1] = {
	NOT_A_PORT,
	NOT_A_PORT,
	_SFR_IO_ADDR(PORTB),
	_SFR_IO_ADDR(PORTC),
	_SFR_IO_ADDR(PORTD),
};

int port_to_input[NUM_PORTS + 1] = {
	NOT_A_PORT,
	NOT_A_PORT,
	_SFR_IO_ADDR(PINB),
	_SFR_IO_ADDR(PINC),
	_SFR_IO_ADDR(PIND),
};

pin_t digital_pin_to_port_array[] = {
	{ NOT_A_PIN, NOT_A_PIN },
	
	{ PC, 6 },
	{ PD, 0 },
	{ PD, 1 },
	{ PD, 2 },
	{ PD, 3 },
	{ PD, 4 },
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
	{ PB, 6 },
	{ PB, 7 },
	{ PD, 5 },
	{ PD, 6 },
	{ PD, 7 },
	{ PB, 0 },
	
	{ PB, 1 },
	{ PB, 2 },
	{ PB, 3 },
	{ PB, 4 },
	{ PB, 5 },
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
	{ NOT_A_PIN, NOT_A_PIN },
	{ PC, 0 },
	{ PC, 1 },
	{ PC, 2 },
	{ PC, 3 },
	{ PC, 4 },
	{ PC, 5 },
};

pin_t *digital_pin_to_port = digital_pin_to_port_array;
pin_t *analog_in_pin_to_port = digital_pin_to_port_array;
pin_t *analog_out_pin_to_port = digital_pin_to_port_array;
