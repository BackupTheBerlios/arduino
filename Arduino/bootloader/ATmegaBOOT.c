/**********************************************************/
/* Serial Bootloader for Atmel megaAVR Controllers        */
/*                                                        */
/* ATmegaBOOT.c                                           */
/*                                                        */
/* build: 050123                                          */
/*                                                        */
/* Hacked by DojoCorp - ZGZ - MMX - IVR                   */
/*                                                        */
/* For the latest version see:                            */
/* http://www.0j0.org and                                 */
/* http://www.potemkin.org								  */
/*                                                        */
/* ------------------------------------------------------ */
/*                                                        */
/* Copyleft (c) 2005, DojoDave                            */
/* Creative Commons   .                                   */
/* see avr1.org for original file and information         */
/*                                                        */
/* This program is free software; you can redistribute it */
/* and/or modify it under the terms of the GNU General    */
/* Public License as published by the Free Software       */
/* Foundation; either version 2 of the License, or        */
/* (at your option) any later version.                    */
/*                                                        */
/* This program is distributed in the hope that it will   */
/* be useful, but WITHOUT ANY WARRANTY; without even the  */
/* implied warranty of MERCHANTABILITY or FITNESS FOR A   */
/* PARTICULAR PURPOSE.  See the GNU General Public        */
/* License for more details.                              */
/*                                                        */
/* You should have received a copy of the GNU General     */
/* Public License along with this program; if not, write  */
/* to the Free Software Foundation, Inc.,                 */
/* 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA */
/*                                                        */
/* Licence can be viewed at                               */
/* http://www.fsf.org/licenses/gpl.txt                    */
/*                                                        */
/* Target = Atmel AVR m128,m64,m32,m16,m8,m162,m163,m169, */
/* m8515,m8535. ATmega161 has a very small boot block so  */
/* isn't supported.                                       */
/*                                                        */
/* Tested with m8 - 						              */
/*                                                        */
/**********************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#define F_CPU			16000000

/* We, Malmoitians, like slow interaction
 * therefore the slow baud rate ;-)
 */
#define BAUD_RATE		9600

/* 6.000.000 is more or less 8 seconds at the
 * speed configured here
 */
#define MAX_TIME_COUNT	6000000

#define DECRYPT 0
#define ENCRYPT 1
/* #define DES_ENCRYPTION */

/* SW_MAJOR and MINOR needs to be updated from time to time to avoid warning message from AVR Studio */
#define HW_VER	 0x02
#define SW_MAJOR 0x01
#define SW_MINOR 0x12


/* Adjust to suit whatever pin your hardware uses to enter the bootloader */


/* other ATmegas have only one UART, so only one pin is defined to enter bootloader */
#define BL_DDR  DDRD
#define BL_PORT PORTD
#define BL_PIN  PIND
#define BL      PIND6

/* define pin for onboard LED */
/* depends on product */

/* Onboard LED is connected to pin PB5 */
#define LED_DDR  DDRB
#define LED_PORT PORTB
#define LED_PIN  PINB
#define LED      PINB5


#define SIG1	0x1E	// Yep, Atmel is the only manufacturer of AVR micros.  Single source :(
#define SIG2	0x93
#define SIG3	0x07
#define PAGE_SIZE	0x20U	//32 words


void putch(char);
char getch(void);
void getNch(uint8_t);
void byte_response(uint8_t);
void nothing_response(void);
char gethex(void);
void puthex(char);

union address_union {
  uint16_t word;
  uint8_t  byte[2];
} address;

union length_union {
  uint16_t word;
  uint8_t  byte[2];
} length;

struct flags_struct {
  unsigned eeprom : 1;
  unsigned rampz  : 1;
} flags;

uint8_t buff[256];
uint8_t address_high;

uint8_t pagesz=0x80;

uint8_t i;
uint8_t bootuart0=0,bootuart1=0;


void (*app_start)(void) = 0x0000;

int main(void)
{
  uint8_t ch,ch2;
  uint16_t w;

  cbi(BL_DDR,BL);
  sbi(BL_PORT,BL);

  asm volatile("nop\n\t");

  /* check if flash is programmed already, if not start bootloader anyway */
  if(pgm_read_byte_near(0x0000) != 0xFF) {

    /* check if bootloader pin is set low */
    //if(bit_is_set(BL_PIN,BL)) app_start();
  }

  /* initialize UART(s) depending on CPU defined */
  /* m8 */
  UBRRH = (((F_CPU/BAUD_RATE)/16)-1)>>8; 	// set baud rate
  UBRRL = (((F_CPU/BAUD_RATE)/16)-1);
  UCSRB =(1<<RXEN)|(1<<TXEN);  // enable Rx & Tx
  UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);  // config USART; 8N1

  //UBRRL = (uint8_t)(F_CPU/(BAUD_RATE*16L)-1);
  //UBRRH = (F_CPU/(BAUD_RATE*16L)-1) >> 8;
  //UCSRA = 0x00;
  //UCSRC = 0x86;
  //UCSRB = _BV(TXEN)|_BV(RXEN);

  putch('\0');

  uint32_t l;
  uint32_t time_count;
  time_count=0;

  /* set LED pin as output */
  sbi(LED_DDR,LED);

  /* flash onboard LED three times to signal entering of bootloader */
  for(i=0; i<3; ++i) {
    for(l=0; l<40000000; ++l);
    sbi(LED_PORT,LED);
    for(l=0; l<40000000; ++l);
    cbi(LED_PORT,LED);
  }

  /* forever */
  for (;;) {
    if((inb(UCSRA) & _BV(RXC))){
    /* get character from UART */
		ch = getch();
		
		/* A bunch of if...else if... gives smaller code than switch...case ! */
	
		/* Hello is anyone home ? */ 
		if(ch=='0') {
		  nothing_response();
		}
	
		if(ch=='l') {
		 if (getch() == 'o') {
			if (getch() == 'l') {
				if (getch() == 'a') {
					app_start();
				}
			}
		 }
		}
	
	   /* Request programmer ID */
		/* Not using PROGMEM string due to boot block in m128 being beyond 64kB boundry  */
		/* Would need to selectively manipulate RAMPZ, and it's only 9 characters anyway so who cares.  */
		else if(ch=='1') {
		  if (getch() == ' ') {
		putch(0x14);
		putch('A');
		putch('V');
		putch('R');
		putch(' ');
		putch('I');
		putch('S');
		putch('P');
		putch(0x10);
		  }
		}
	
		/* AVR ISP/STK500 board commands  DON'T CARE so default nothing_response */
		else if(ch=='@') {
		  ch2 = getch();
		  if (ch2>0x85) getch();
		  nothing_response();
		}
	
		/* AVR ISP/STK500 board requests */
		else if(ch=='A') {
		  ch2 = getch();
		  if(ch2==0x80) byte_response(HW_VER);		// Hardware version
		  else if(ch2==0x81) byte_response(SW_MAJOR);	// Software major version
		  else if(ch2==0x82) byte_response(SW_MINOR);	// Software minor version
		  else if(ch2==0x98) byte_response(0x03);		// Unknown but seems to be required by avr studio 3.56
		  else byte_response(0x00);				// Covers various unnecessary responses we don't care about
		}
	
		/* Device Parameters  DON'T CARE, DEVICE IS FIXED  */
		else if(ch=='B') {
		  getNch(20);
		  nothing_response();
		}
	
		/* Parallel programming stuff  DON'T CARE  */
		else if(ch=='E') {
		  getNch(5);
		  nothing_response();
		}
	
		/* Enter programming mode  */
		else if(ch=='P') {
		  nothing_response();
		}
	
		/* Leave programming mode  */
		else if(ch=='Q') {
		  nothing_response();
		}
	
		/* Erase device, don't care as we will erase one page at a time anyway.  */
		else if(ch=='R') {
		  nothing_response();
		}
	
		/* Set address, little endian. EEPROM in bytes, FLASH in words  */
		/* Perhaps extra address bytes may be added in future to support > 128kB FLASH.  */
		/* This might explain why little endian was used here, big endian used everywhere else.  */
		else if(ch=='U') {
		  address.byte[0] = getch();
		  address.byte[1] = getch();
		  nothing_response();
		}
	
		/* Universal SPI programming command, disabled.  Would be used for fuses and lock bits.  */
		else if(ch=='V') {
		  getNch(4);
		  byte_response(0x00);
		}
	
		/* Write memory, length is big endian and is in bytes  */
		else if(ch=='d') {
		  length.byte[1] = getch();
		  length.byte[0] = getch();
		  flags.eeprom = 0;
		  if (getch() == 'E') flags.eeprom = 1;
		  for (w=0;w<length.word;w++) {
		buff[w] = getch();	                        // Store data in buffer, can't keep up with serial data stream whilst programming pages
		  }
		  if (getch() == ' ') {
		if (flags.eeprom) {		                //Write to EEPROM one byte at a time
		  for(w=0;w<length.word;w++) {
			eeprom_wb(address.word,buff[w]);
			address.word++;
		  }			
		}
		else {					        //Write to FLASH one page at a time
		  if (address.byte[1]>127) address_high = 0x01;	//Only possible with m128, m256 will need 3rd address byte. FIXME
		  else address_high = 0x00;
	
		  address.word = address.word << 1;	        //address * 2 -> byte location
		  /* if ((length.byte[0] & 0x01) == 0x01) length.word++;	//Even up an odd number of bytes */
		  if ((length.byte[0] & 0x01)) length.word++;	//Even up an odd number of bytes
		  cli();					//Disable interrupts, just to be sure
		  while(bit_is_set(EECR,EEWE));			//Wait for previous EEPROM writes to complete
		  asm volatile("clr	r17		\n\t"	//page_word_count
				   "lds	r30,address	\n\t"	//Address of FLASH location (in bytes)
				   "lds	r31,address+1	\n\t"
				   "ldi	r28,lo8(buff)	\n\t"	//Start of buffer array in RAM
				   "ldi	r29,hi8(buff)	\n\t"
				   "lds	r24,length	\n\t"	//Length of data to be written (in bytes)
				   "lds	r25,length+1	\n\t"
				   "length_loop:		\n\t"	//Main loop, repeat for number of words in block							 							 
				   "cpi	r17,0x00	\n\t"	//If page_word_count=0 then erase page
				   "brne	no_page_erase	\n\t"						 
				   "wait_spm1:		\n\t"
				   "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				   "andi	r16,1           \n\t"
				   "cpi	r16,1           \n\t"
				   "breq	wait_spm1       \n\t"
				   "ldi	r16,0x03	\n\t"	//Erase page pointed to by Z
				   "sts	%0,r16		\n\t"
				   "spm			\n\t"							 
				   "wait_spm2:		\n\t"
				   "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				   "andi	r16,1           \n\t"
				   "cpi	r16,1           \n\t"
				   "breq	wait_spm2       \n\t"									 
	
				   "ldi	r16,0x11	\n\t"	//Re-enable RWW section
				   "sts	%0,r16		\n\t"						 			 
				   "spm			\n\t"
				   "no_page_erase:		\n\t"							 
				   "ld	r0,Y+		\n\t"	//Write 2 bytes into page buffer
				   "ld	r1,Y+		\n\t"							 
								 
				   "wait_spm3:		\n\t"
				   "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				   "andi	r16,1           \n\t"
				   "cpi	r16,1           \n\t"
				   "breq	wait_spm3       \n\t"
				   "ldi	r16,0x01	\n\t"	//Load r0,r1 into FLASH page buffer
				   "sts	%0,r16		\n\t"
				   "spm			\n\t"
								 
				   "inc	r17		\n\t"	//page_word_count++
				   "cpi r17,%1	        \n\t"
				   "brlo	same_page	\n\t"	//Still same page in FLASH
				   "write_page:		\n\t"
				   "clr	r17		\n\t"	//New page, write current one first
				   "wait_spm4:		\n\t"
				   "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				   "andi	r16,1           \n\t"
				   "cpi	r16,1           \n\t"
				   "breq	wait_spm4       \n\t"
				   "ldi	r16,0x05	\n\t"	//Write page pointed to by Z
				   "sts	%0,r16		\n\t"
				   "spm			\n\t"
				   "wait_spm5:		\n\t"
				   "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				   "andi	r16,1           \n\t"
				   "cpi	r16,1           \n\t"
				   "breq	wait_spm5       \n\t"									 
				   "ldi	r16,0x11	\n\t"	//Re-enable RWW section
				   "sts	%0,r16		\n\t"						 			 
				   "spm			\n\t"					 		 
				   "same_page:		\n\t"							 
				   "adiw	r30,2		\n\t"	//Next word in FLASH
				   "sbiw	r24,2		\n\t"	//length-2
				   "breq	final_write	\n\t"	//Finished
				   "rjmp	length_loop	\n\t"
				   "final_write:		\n\t"
				   "cpi	r17,0		\n\t"
				   "breq	block_done	\n\t"
				   "adiw	r24,2		\n\t"	//length+2, fool above check on length after short page write
				   "rjmp	write_page	\n\t"
				   "block_done:		\n\t"
				   "clr	__zero_reg__	\n\t"	//restore zero register
				   : "=m" (SPMCR) : "M" (PAGE_SIZE) : "r0","r16","r17","r24","r25","r28","r29","r30","r31");
	
		  /* Should really add a wait for RWW section to be enabled, don't actually need it since we never */
		  /* exit the bootloader without a power cycle anyhow */
		}
		putch(0x14);
		putch(0x10);
		  }		
		}
	
		/* Read memory block mode, length is big endian.  */
		else if(ch=='t') {
		  length.byte[1] = getch();
		  length.byte[0] = getch();
		  if (getch() == 'E') flags.eeprom = 1;
		  else {
		flags.eeprom = 0;
		address.word = address.word << 1;	        // address * 2 -> byte location
		  }
		  if (getch() == ' ') {		                // Command terminator
		putch(0x14);
		for (w=0;w < length.word;w++) {		        // Can handle odd and even lengths okay
		  if (flags.eeprom) {	                        // Byte access EEPROM read
			putch(eeprom_rb(address.word));
			address.word++;
		  }
		  else {
	
			if (!flags.rampz) putch(pgm_read_byte_near(address.word));
			address.word++;
		  }
		}
		putch(0x10);
		  }
		}
	
		/* Get device signature bytes  */
		else if(ch=='u') {
		  if (getch() == ' ') {
		putch(0x14);
		putch(SIG1);
		putch(SIG2);
		putch(SIG3);
		putch(0x10);
		  }
		}
	
		/* Read oscillator calibration byte */
		else if(ch=='v') {
		  byte_response(0x00);
		}
    }
		else {
		time_count++;
		if (time_count>=MAX_TIME_COUNT) {
			app_start();
		}
	}

 }
  /* end of forever loop */

}


char gethex(void) {
  char ah,al;

  ah = getch(); putch(ah);
  al = getch(); putch(al);
  if(ah >= 'a') {
    ah = ah - 'a' + 0x0a;
  } else if(ah >= '0') {
    ah -= '0';
  }
  if(al >= 'a') {
    al = al - 'a' + 0x0a;
  } else if(al >= '0') {
    al -= '0';
  }
  return (ah << 4) + al;
}

void puthex(char ch) {
  char ah,al;

  ah = (ch & 0xf0) >> 4;
  if(ah >= 0x0a) {
    ah = ah - 0x0a + 'a';
  } else {
    ah += '0';
  }
  al = (ch & 0x0f);
  if(al >= 0x0a) {
    al = al - 0x0a + 'a';
  } else {
    al += '0';
  }
  putch(ah);
  putch(al);
}

void putch(char ch)
{
  /* m8 */
  while (!(inb(UCSRA) & _BV(UDRE)));
  outb(UDR,ch);
}

char getch(void)
{
  /* m8 */
  while(!(inb(UCSRA) & _BV(RXC))) {
	/* HACKME:: here is a good place to count times*/
  }
  return (inb(UDR));
}

void getNch(uint8_t count)
{
  uint8_t i;
  for(i=0;i<count;i++) {
    /* m8 */
    while(!(inb(UCSRA) & _BV(RXC)));
    inb(UDR);
  }
}

void byte_response(uint8_t val)
{
  if (getch() == ' ') {
    putch(0x14);
    putch(val);
    putch(0x10);
  }
}

void nothing_response(void)
{
  if (getch() == ' ') {
    putch(0x14);
    putch(0x10);
  }
}

/* end of file ATmegaBOOT.c */



