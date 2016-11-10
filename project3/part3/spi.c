#include "MKL25Z4.h"
#include "spi.h"


void SPI0_init(void) {
	SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
	PORTD->PCR[1] = 0x200; /* make PTD1 pin as SPI SCK */
	PORTD->PCR[2] = 0x200; /* make PTD2 pin as SPI MOSI */
	PORTD->PCR[3] = 0x200; /* make PTD3 pin as SPI MISO */
	PORTD->PCR[0] = 0x100; /* make PTD0 pin as GPIO */
	PORTD->PCR[5] = 0x100; /* make PTD5 pin as GPIO */
	PTD->PDDR |= 0x01; /* make PTD0 as output pin for /SS */
	PTD->PDDR |= 0x01<<5; /* make PTD5 as output pin for /SS */
	PTD->PSOR = 0x01; /* make PTD0 idle high */
	PTD->PCOR = 0x01<<5; /* make PTD5 low */
	SIM->SCGC4 |= 0x400000; /* enable clock to SPI0 */
	SPI0->C1 = 0x10; /* disable SPI and make SPI0 master */
	SPI0->BR = 0x90; /* set Baud rate to 1 MHz */
	SPI0->C1 |= 0x40; /* Enable SPI module */
	}

uint8_t SPI0_write(unsigned char data) {
	volatile char dummy;
	//PTD->PCOR = 1; /* assert /SS */
	while(!(SPI0->S & 0x20)) { } /* wait until tx ready */
	SPI0->D = data; /* send data byte */
	while(!(SPI0->S & 0x80)) { } /* wait until tx complete */
	dummy = SPI0->D; /* clear SPRF */
	//PTD->PSOR = 1; /* deassert /SS */
	return dummy;
	}
/* spi.c
 *
 *  Created on: Nov 9, 2016
 *      Author: dhth3498
 */




