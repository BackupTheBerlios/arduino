/*
wiring.c

v 0.1 by David A Mellis
v 0.2 addition by Massimo Banzi
v 0.3 cbi & sbi by Nick Zambetti; 
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
#include "global.h"
#include "a2d.h"
#include "timer.h"
#include "uart.h"

// timer.h #defines delay to be delay_us, we need to undefine
// it so our delay can be in milliseconds.
#undef delay

#include "BConstants.h"
#include "wiring.h"

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

int analogInPinToBit(int pin)
{
	return analog_in_pin_to_port[pin].bit;
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

int analogRead(int pin)
{	
	int ch = analogInPinToBit(pin);
	a2dSetChannel(ch);
	a2dStartConvert();
	while (!a2dIsComplete());
	return ADCW;
	//return a2dConvert10bit(ch);
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
}

void serialWrite(unsigned char c)
{
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

int main(void)
{
	sei();
	
	// timer 0 is used for millis() and delay()
	timer0Init();

	// timer 1 is used for the hardware pwm
	timer1Init();
	timer1SetPrescaler(TIMER_CLK_DIV1);
	timer1PWMInit(8);
	timer1PWMAOn();
	timer1PWMBOn();

	a2dInit();
	a2dSetPrescaler(ADC_PRESCALE_DIV128);
	
	setup();
	
	for (;;)
		loop();
		
	return 0;
}
