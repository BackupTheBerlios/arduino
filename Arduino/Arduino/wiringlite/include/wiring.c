/*

wiring.c

v0.1 by David A Mellis
v0.2 addition by Massimo Banzi
v0.3 Nick Zambetti: added cbi & sbi
     DAM: ported delay, millis, analogWrite, and serial routines to avrlib

Contains code by
Peter Fleury.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/delay.h>
#include <stdio.h>
#include <stdarg.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// from Pascal's avrlib
#include "timer.h"

// timer.h #defines delay to be delay_us, we need to undefine
// it so our delay can be in milliseconds.
#undef delay

#include "BConstants.h"
#include "wiring.h"
#include "uart.h"

//int timer0 = 0;
//unsigned long msCount = 0;

int digitalPinToPort(int pin)
{
	return digital_pin_to_port[pin].port;
}

int digitalPinToBit(int pin)
{
	return digital_pin_to_port[pin].bit;
}

int analogOutPinToPort(int pin)
{
	return analog_out_pin_to_port[pin].port;
}

int analogOutPinToBit(int pin)
{
	return analog_out_pin_to_port[pin].bit;
}

void pinMode(int pin, int mode)
{
	if (digitalPinToPort(pin) != NOT_A_PIN) {
		if (mode == INPUT)
			cbi(_SFR_IO8(port_to_mode[digitalPinToPort(pin)]), digitalPinToBit(pin));
		else
			sbi(_SFR_IO8(port_to_mode[digitalPinToPort(pin)]), digitalPinToBit(pin));
	}
}

void digitalWrite(int pin, int val)
{
	if (digitalPinToPort(pin) != NOT_A_PIN) {
		if (val == LOW)
			cbi(_SFR_IO8(port_to_output[digitalPinToPort(pin)]), digitalPinToBit(pin));
		else
			sbi(_SFR_IO8(port_to_output[digitalPinToPort(pin)]), digitalPinToBit(pin));
	}
}

int digitalRead(int pin)
{
	if (digitalPinToPort(pin) != NOT_A_PIN)
		return (_SFR_IO8(port_to_input[digitalPinToPort(pin)]) >> digitalPinToBit(pin)) & 0x01;
	
	return LOW;
}

int analogRead(int channel)
{	   
    // Select pin ADC0 using MUX
    ADMUX = channel;
    
    //Start conversion
    ADCSRA |= _BV(ADSC);
    
    // wait until converstion completed
    while (ADCSRA & _BV(ADSC)) {}
    
    // get converted value
    return ADCW;  
}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_... file.  For the rest of the pins, we default
// to digital output.
void analogWrite(int pin, int val)
{
	if (analogOutPinToBit(pin) == 1)
		timer1PWMASet(val);
	else if (analogOutPinToBit(pin) == 2)
		timer1PWMBSet(val);
	else if (val < 128)
		digitalWrite(pin, LOW);
	else
		digitalWrite(pin, HIGH);
}

void beginSerial(int baud)
{
	uartInit();
	uartSetBaudRate(baud);
    //uart_init(UART_BAUD_SELECT(baud, F_CPU));
}

void serialWrite(unsigned char c)
{
	//uart_putc(c);
	uartSendByte(c);
}

void printMode(int mode)
{
	// do nothing
}

void uartSendString(unsigned char *str)
{
	while (*str)
		uartSendByte(*str++);
}

void print(const char *format, ...)
{
	char buf[256];
	va_list ap;
	
	va_start(ap, format);
	vsnprintf(buf, 256, format, ap);
	va_end(ap);
	
	uartSendString(buf);
}

unsigned long millis()
{
	// timer 0 increments every timer0GetPrescaler() cycles, and overflows when it
	// reaches 256.  we calculate the total number of clock cycles, then divide
	// by the number of clock cycles per millisecond.
	return timer0GetOverflowCount() * timer0GetPrescaler() * 256L / F_CPU * 1000L;
}

void delay(unsigned long ms)
{
	timerPause(ms);
}

/*
unsigned long millis()
{
	return msCount;
}

void delay(unsigned long ms)
{
    int x = ms / 16;
	
	
	while ( x ) {
	  x--;
      _delay_loop_2(0);
	}
     
	//delay_ms(ms);
	
	//unsigned long end = millis() + ms;
	
	//while (millis() != end) {}
}

void delay_ms(unsigned short ms)
// delay for a minimum of <ms> 
// with a 4Mhz crystal, the resolution is 1 ms 
{
	unsigned short outer1, outer2;
     	outer1 = 5000; 

    	while (outer1) {
		outer2 = 5000;
		while (outer2) {
			while ( ms ) ms--;
			outer2--;
		}
		outer1--;
	}
}

void delay(unsigned long ms) {
    unsigned long outer1, outer2;
    outer1 = 400;
    while (outer1) {
        outer2 = 1000;
        while (outer2) {
            while ( ms ) ms--;
            outer2--;
        }
        outer1--;
    }
}

void timer0Init(void)
{
	TCCR0 = 0x03; // prescale of 64
	TIMSK |= 0x01;
}

INTERRUPT(SIG_OVERFLOW0)
{
	++msCount;
	TCNT0 = -(F_CPU / 64000UL);
}
*/

int main(void)
{
	// Activate ADC with Prescaler 16 --> 1Mhz/16 = 62.5kHz
	ADCSRA = _BV(ADEN) | _BV(ADPS2);
	//ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1);
	
	sei();
	
	// timer 0 is used for millis() and delay()
	timer0Init();

	// timer 1 is used for the hardware pwm
	timer1Init();
	timer1SetPrescaler(TIMER_CLK_DIV1);
	timer1PWMInit(8);
	timer1PWMAOn();
	timer1PWMBOn();
	
	setup();
	
	for (;;)
		loop();
		
	return 0;
}
