
#include "pwmled.h"
void led_uart (void) {
__disable_irq(); /* global disable IRQs */
UART0_init();
LED_init();
__enable_irq(); /* global enable IRQs */
while (1) {
 }
}
/* UART0 interrupt handler */
void UART0_IRQHandler(void) {
char c;
c = UART0->D;     // receive characters
LEDbright(c);
 }
/* initialize UART0 to receive at 115200 Baud */
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
}
/* initialize all three LEDs on the FRDM board */
void LED_init(void)
{
SIM->SCGC6 |=	0x04000000;//clock for tpm0
SIM->SCGC6 |=	0x01000000;//clock for tpm1
SIM->SOPT2 |=   0x01000000; //
TPM2->SC  |= TPM_SC_CMOD(1)|TPM_SC_PS(7);
TPM2->MOD=1875;
TPM0->SC  |= TPM_SC_CMOD(1)|TPM_SC_PS(7);
TPM0->MOD=1875;

SIM->SCGC5 |= 0x400; /* enable clock to Port B */
SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
PORTB->PCR[18] = PORT_PCR_MUX(3); /* make PTB18 pin as GPIO */
PORTB->PCR[19] =PORT_PCR_MUX(3); /* make PTB19 pin as GPIO */
PORTD->PCR[1] = PORT_PCR_MUX(4); /* make PTD1 pin as GPIO */
TPM2->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK |TPM_CnSC_ELSA_MASK;
      TPM2->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK |TPM_CnSC_ELSA_MASK;
      TPM0->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK |TPM_CnSC_ELSA_MASK;

}
void LEDbright(char value)
{   LEDON(value,initial);
	if (value == 'i' && initial <1000)   // increase brightness if char i
		initial += 100;
	else if(value == 'd' && initial >0)
		initial -= 100;    //decrease brightness if char d

	LEDON(value,initial);
}
void LEDON(char value, uint16_t intensity)
{
 if(value =='r')
    {
	TPM2->CONTROLS[0].CnV = intensity;   // red LED with variable intensity
	TPM2->CONTROLS[1].CnV = 0;
	TPM0->CONTROLS[1].CnV = 0;
	x=1;
	}
	else if(value =='y')
	{
	TPM2->CONTROLS[0].CnV = TPM2->CONTROLS[1].CnV = intensity; // red and green
	TPM0->CONTROLS[1].CnV = 0;
	x=2;
	}
	else if(value =='g')   //green
	{
	TPM2->CONTROLS[1].CnV = intensity;
	TPM2->CONTROLS[0].CnV = 0;
	TPM0->CONTROLS[1].CnV = 0;
	x=3;
	}
	else if(value =='b')    // blue
	{
	TPM0->CONTROLS[1].CnV = intensity;
	TPM2->CONTROLS[1].CnV = 0;
	TPM2->CONTROLS[0].CnV = 0;
	x=4;
	}
	else if(value =='m')    //magenta
	{
	TPM2->CONTROLS[0].CnV = TPM0->CONTROLS[1].CnV = intensity;
	TPM2->CONTROLS[1].CnV = 0;
	x=5;
	}
	else if(value =='c')   //cyan
	{
	TPM2->CONTROLS[1].CnV = TPM0->CONTROLS[1].CnV = intensity;
	TPM2->CONTROLS[0].CnV = 0;
	x=6;
	}
	else if(value =='w')  // white- all three
	{
	TPM2->CONTROLS[1].CnV = TPM2->CONTROLS[0].CnV=TPM0->CONTROLS[1].CnV = intensity;
	x=7;
	}
	else if(value ==('i') || value=='d')   // increasing or decreasing brightness
	{ if (x==1)
		      {
		    	  TPM2->CONTROLS[0].CnV = intensity;
		    	  	TPM2->CONTROLS[1].CnV = 0;
		    	  	TPM0->CONTROLS[1].CnV = 0;

		      }
		      else if (x==2)
		      {
		    	  TPM2->CONTROLS[0].CnV = TPM2->CONTROLS[1].CnV = intensity;
		    	  	TPM0->CONTROLS[1].CnV = 0;


		      }
		      else if (x==3)
		      {
		    	  TPM2->CONTROLS[1].CnV = intensity;
		    	  	TPM2->CONTROLS[0].CnV = 0;
		    	  	TPM0->CONTROLS[1].CnV = 0;


		      }
		      else if (x==4)
		      {
		    	  TPM0->CONTROLS[1].CnV = intensity;
		    	  	TPM2->CONTROLS[1].CnV = 0;
		    	  	TPM2->CONTROLS[0].CnV = 0;


		      }
		      else if (x==5)
		      {
		    		TPM2->CONTROLS[0].CnV = TPM0->CONTROLS[1].CnV = intensity;
		    		TPM2->CONTROLS[1].CnV = 0;


		      }
		      else if (x==6)
		      {
		    	  TPM2->CONTROLS[1].CnV = TPM0->CONTROLS[1].CnV = intensity;
		    	  	TPM2->CONTROLS[0].CnV = 0;


		      }
		      else if (x==7){
		    	  TPM2->CONTROLS[1].CnV = TPM2->CONTROLS[0].CnV=TPM0->CONTROLS[1].CnV = intensity;

		      }    }
		else
     	{
		TPM2->CONTROLS[0].CnV = 0;
		TPM2->CONTROLS[0].CnV = 0;
		TPM0->CONTROLS[1].CnV = 0;
        }
}




