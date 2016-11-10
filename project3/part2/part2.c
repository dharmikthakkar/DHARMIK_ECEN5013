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

#include "MKL25Z4.h"
#define MAX_DATA_SIZE 10
#define PASS 1
#define FAIL 0

typedef enum Cmds_t {GET_TEMP = 0x01,// Provides a temperature Reading
	SET_SPEED= 0x02,// Sets speed for a motor Controller
	LED_CONFIG= 0x03,// Configures the LEDs to some color
	SET_LPM=0x04,// Sets Low Power Mode
}Cmds;

typedef struct CI_Msg_t{Cmds command;   // Command operation
	uint8_t length; // Size of message
	uint8_t data[MAX_DATA_SIZE];// Generic data payload
	uint16_t checksum;// Error detection checksum
}CI_MSG;

static int i = 0;
uint8_t rxdata[2*MAX_DATA_SIZE];
uint8_t *rxptr;
uint8_t rxmessage[MAX_DATA_SIZE];
uint8_t *rxmessageptr;
uint8_t irx=0;
uint8_t test_message=0;
uint8_t rx_complete=0;
uint8_t msb_part=0, lsb_part=0;
uint8_t temp_irx;
uint8_t rx_message_byte=0, rx_message_data=0;

CI_MSG CI_MSG_STRUCT;

//void Decode_CI_Msg(CI_MSG * msg);
void UART0_init(void);
void Decode_CI_Msg(CI_MSG * ms);
uint16_t check_message(CI_MSG * ms);
void get_temp(CI_MSG * ms);
void set_speed(CI_MSG * ms);
void LED_config(CI_MSG * ms);
void set_lpm(CI_MSG * ms);
void error_message();



void Decode_CI_Msg(CI_MSG * ms){
	uint16_t message_checksum=check_message(ms);
	if(message_checksum == ms->checksum){
		if(ms->command == GET_TEMP){
			get_temp(ms);
		}
		else if(ms->command == SET_SPEED){
			set_speed(ms);
		}
		else if(ms->command == LED_CONFIG){
			LED_config(ms);
		}
		else if(ms->command == SET_LPM){
			set_lpm(ms);
		}
	}
	else error_message();
}

uint16_t check_message(CI_MSG * ms){
	uint16_t msg_checksum=0;
	for(uint8_t msgi=0; msgi<ms->length-1;msgi++){
		msg_checksum+=(uint8_t)*(rxmessageptr+msgi);
		}
	return msg_checksum;
}

void get_temp(CI_MSG * ms){

}
void set_speed(CI_MSG * ms){

}
void LED_config(CI_MSG * ms){

}
void set_lpm(CI_MSG * ms){

}
void error_message(){

}

void UART0_init(void) {
SIM->SCGC4 |= 0x0400; /* enable clock for UART0 */
SIM->SOPT2 |= 0x04000000; /* use FLL output for UART Baud rate generator */
UART0->C2 = 0; /* turn off UART0 while changing configurations */
UART0->BDH = 0x00;
UART0->BDL = 0x17; /* 115200 Baud */
UART0->C4 = 0x0F; /* Over Sampling Ratio 16 */
UART0->C1 = 0x00; /* 8-bit data */
UART0->C2 = 0x24; /* enable receive and receive interrupt*/
NVIC->ISER[0] |= 0x00001000; /* enable INT12 (bit 12 of ISER[0]) */
SIM->SCGC5 |= 0x0200; /* enable clock for PORTA */
PORTA->PCR[1] = 0x0200; /* make PTA1 UART0_Rx pin */
//tx=0;
}

/*
UART0 interrupt handler */
void UART0_IRQHandler(void) {
uint8_t diff_lsb, diff_msb;
*(rxptr+irx)= UART0->D; /* read the char received */
temp_irx=irx;
if(temp_irx%2){
	if(*(rxptr+irx)>=0x30 && *(rxptr+irx)<=0x39) diff_lsb=0x30;
	else if(*(rxptr+irx)>=0x41 && *(rxptr+irx)<=0x46) diff_lsb=0x37;
	else if(*(rxptr+irx)>=0x61 && *(rxptr+irx)<=0x66) diff_lsb=0x57;
	else error_message();
	if(*(rxptr+irx-1)>=0x30 && *(rxptr+irx-1)<=0x39) diff_msb=0x30;
	else if(*(rxptr+irx-1)>=0x41 && *(rxptr+irx-1)<=0x46) diff_msb=0x37;
	else if(*(rxptr+irx-1)>=0x61 && *(rxptr+irx-1)<=0x66) diff_msb=0x57;
	else error_message();
	lsb_part=(*(rxptr+irx)-diff_lsb)&0x0F;
	msb_part=((*(rxptr+irx-1)-diff_msb)<<4)&0xF0;
	*(rxmessageptr+rx_message_byte-1)=msb_part | lsb_part;
	if(rx_message_byte==1){
		CI_MSG_STRUCT.command=msb_part | lsb_part;
	}
	else if(rx_message_byte==2){
		CI_MSG_STRUCT.length=msb_part | lsb_part;
	}
	else if(rx_message_byte>2 && rx_message_data<CI_MSG_STRUCT.length-3){
		CI_MSG_STRUCT.data[rx_message_data]=msb_part | lsb_part;
		rx_message_data++;
	}
	else{
		CI_MSG_STRUCT.checksum=msb_part | lsb_part;
		__disable_irq();
		rx_complete=1;
	}
}
else rx_message_byte++;
irx++;

}

int main(void)
{
	rxptr=rxdata;
	rxmessageptr=rxmessage;

	//CI_MSG CI_MSG_STRUCT;

	__disable_irq(); /* global disable IRQs */
	UART0_init();
	__enable_irq();

	while(rx_complete==0);
	Decode_CI_Msg(&CI_MSG_STRUCT);
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
