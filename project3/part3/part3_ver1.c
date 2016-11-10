/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



/*P8_1.c: Send ‘A’ to ‘Z’ via SPI0
* PTD1 pin as SPI SCK
* PTD2 pin as SPI MOSI
* PTD3 pin as SPI MISO
* PTD0 pin as SPI CSN
* PTD5 pin as SPI CE
*/
#include "MKL25Z4.h"
#include "nrf.h"
#define dataLen 3

void SPI0_init(void);
uint8_t SPI0_write(unsigned char data);
/*
void TPM0_IRQHandler(void){
	TPM0->SC|=0x80;				//clear TOF
	count+=1;
}
*/
void TIMER0_init(){
		//__disable_irq();

		SIM->SOPT2|=0x01000000;		//use mcgfllclk
		SIM->SCGC6|=0x01000000;		//enable clock to tpm0
		TPM0->SC=0;					//disable timer
		TPM0->SC=0x06; 				//prescaler to 64
		TPM0->MOD=0x04;
		TPM0->SC|=0x80;				//clear TOF
		//TPM0->SC|=0x40;				//enable timeout interrupt
		//NVIC->ISER[0]|=0x00020000;
		//__enable_irq();
}

void delay(){
	TPM0->SC|=0x08;				//enable timer
	while((TPM0->SC & 0x80) == 0) { }/* wait until the TOF is set */
	TPM0->SC |= 0x80; /* clear TOF */
}


uint8_t *WriteToNrf(uint8_t ReadWrite, uint8_t reg, uint8_t *val, uint8_t antVal)
{
	__disable_irq();	//disable global interrupt

	if (ReadWrite == W)
	{
		reg = W_REGISTER + reg;	//ex: reg = EN_AA: 0b0010 0000 + 0b0000 0001 = 0b0010 0001
	}

	//Static uint8_t för att det ska gå att returnera en array (lägg märke till "*" uppe på funktionen!!!)
	static uint8_t ret[dataLen];	//antar att det längsta man vill läsa ut när man kallar på "R" är dataleng-långt, dvs använder man bara 1byte datalengd å vill läsa ut 5byte RF_Adress så skriv 5 här ist!!!

	delay();		//alla delay är så att nrfen ska hinna med! (microsekunder)
	PTD->PCOR = 0x01; /* make PTD0 low */	//CSN low = nrf-chippet börjar lyssna
	delay();
	SPI0_write(reg);	//första SPI-kommandot efter CSN-låg berättar för nrf'en vilket av dess register som ska redigeras ex: 0b0010 0001 write to registry EN_AA
	delay();

	int i;
	for(i=0; i<antVal; i++)
	{
		if (ReadWrite == R && reg != W_TX_PAYLOAD)
		{
			ret[i]=SPI0_write(NOP);	//Andra och resten av SPI kommandot säger åt nrfen vilka värden som i det här fallet ska läsas
			delay();
		}
		else
		{
			SPI0_write(val[i]);	//Andra och resten av SPI kommandot säger åt nrfen vilka värden som i det här fallet ska skrivas till
			delay();
		}
	}
	PTD->PSOR = 0x01; /* make PTD0 idle high */	//CSN IR_High = nrf-chippet slutar lyssna

	__enable_irq();//enable global interrupt

	return ret;	//returnerar en array
}

int main(void) {
	unsigned char c;
	SPI0_init(); /* enable SPI0 */
	TIMER0_init();
	delay();		//10us delay
	PTD->PCOR = 0x01; /* make PTD0 low */
	delay();		//10us delay
	uint8_t val[5];	//en array av integers som skickar värden till WriteToNrf-funktionen

/*
		val[0]=0x01;
		WriteToNrf(W, EN_AA, val, 1);

		val[0]=0x2F;	//0b0010 00011 "2" sets it up to 750uS delay between every retry (at least 500us at 250kbps and if payload >5bytes in 1Mbps, and if payload >15byte in 2Mbps) "F" is number of retries (1-15, now 15)
		WriteToNrf(W, SETUP_RETR, val, 1);

		WriteToNrf(R, SETUP_RETR, val, 1);

			val[0]=0x01;
			WriteToNrf(W, EN_RXADDR, val, 1); //enable data pipe 0


				val[0]=0x03;
				WriteToNrf(W, SETUP_AW, val, 1);


				val[0]=0x01;
				WriteToNrf(W, RF_CH, val, 1); //RF channel registry 0b0000 0001 = 2,401GHz


				val[0]=0x07;
				WriteToNrf(W, RF_SETUP, val, 1);


				int i;
				for(i=0; i<5; i++)
				{
					val[i]=0x12;	//RF channel registry 0b10101011 x 5
				}
				WriteToNrf(W, RX_ADDR_P0, val, 5); //0b0010 1010 write registry

				//TX RF_Adress setup 5 byte
				for(i=0; i<5; i++)
				{
					val[i]=0x12;	//RF channel registry 0b10111100 x 5
				}
				WriteToNrf(W, TX_ADDR, val, 5);

				// payload width setup
				val[0]=dataLen;		//"0b0000 0001"=1 byte per 5byte RF_Adress
				WriteToNrf(W, RX_PW_P0, val, 1);

				//CONFIG reg setup
				val[0]=0x1E;  //0b0000 1110 config registry	bit "1":1=power up,  bit "0":0=transmitter (bit "0":1=Reciver) (bit "4":1=>mask_Max_RT
				WriteToNrf(W, CONFIG, val, 1);
*/

	}

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
