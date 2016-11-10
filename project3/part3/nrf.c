/*
 * nrf.c
 *
 *  Created on: Nov 9, 2016
 *      Author: dhth3498
 */
#include "MKL25Z4.h"
#include "nrf.h"
#include "spi.h"
#define dataLen 3


uint8_t val[5];

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


	static uint8_t ret[dataLen];
	delay();
	PTD->PCOR = 0x01; /* make PTD0 low */
	delay();
	SPI0_write(reg);
	delay();

	int i;
	for(i=0; i<antVal; i++)
	{
		if (ReadWrite == R && reg != W_TX_PAYLOAD)
		{
			ret[i]=SPI0_write(NOP);
			delay();
		}
		else
		{
			SPI0_write(val[i]);
			delay();
		}
	}
	PTD->PSOR = 0x01; /* make PTD0 idle high */	//CSN IR_High

	__enable_irq();//enable global interrupt

	return ret;
}

void nrf_config(){
	val[0]=0x1E;  //0b0000 1110 config registry	bit "1":1=power up,  bit "0":0=transmitter (bit "0":1=Reciver) (bit "4":1=>mask_Max_RT)
	WriteToNrf(W, CONFIG, val, 1);
}

void nrf_set_tx_address(){
	//TX RF_Adress setup 5 byte
	int i;
	for(i=0; i<5; i++)
			{
				val[i]=0x12;	//RF channel registry 0b10111100 x 5
			}
	WriteToNrf(W, TX_ADDR, val, 5);
}



