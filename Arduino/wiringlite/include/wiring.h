/*
 * wiring.h
 * David A. Mellis
 * 21 April 2005
 */

#ifndef Wiring_h
#define Wiring_h

#define NOT_A_PIN 0
#define NOT_A_PORT -1

void delay(unsigned long);
void delay_ms(unsigned short ms);
void pinMode(int, int);
void digitalWrite(int, int);
int digitalRead(int);
void analogWrite(int, int);
int analogRead(int);
unsigned long millis(void);
void setup(void);
void loop(void);
void beginSerial(int);
void serialWrite(unsigned char);

typedef struct {
	int port;
	int bit;
} pin_t;

extern int port_to_mode[];
extern int port_to_input[];
extern int port_to_output[];
extern pin_t *digital_pin_to_port;
extern pin_t *analog_in_pin_to_port;
extern pin_t *analog_out_pin_to_port;

#endif
