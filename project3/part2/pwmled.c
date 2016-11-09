#include <MKL25Z4.H>
#include "pwmled.h"

#define MESSAGE

int x;
uint16_t initial =100;

void UART0_init_pwmled(void);
void delayMs(int n);
void LED_init(void);
void LED_set(char value);
void LEDbright(char value);
void LEDON(char value, uint16_t intensity);

/* initialize UART0 to receive at 115200 Baud */
void UART0_init_pwmled(void) {
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
//PTB->PDDR |= 0x40000; /* make PTB18 as output pin */
//PTB->PSOR |= 0x40000; /* turn off red LED */
PORTB->PCR[19] =PORT_PCR_MUX(3); /* make PTB19 pin as GPIO */
//PTB->PDDR |= 0x80000; /* make PTB19 as output pin */
//PTB->PSOR |= 0x80000; /* turn off green LED */
PORTD->PCR[1] = PORT_PCR_MUX(4); /* make PTD1 pin as GPIO */
//PTD->PDDR |= 0x02; /* make PTD1 as output pin */
//PTD->PSOR |= 0x02; /* turn off blue LED */
TPM2->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK |TPM_CnSC_ELSA_MASK;
      TPM2->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK |TPM_CnSC_ELSA_MASK;
      TPM0->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK |TPM_CnSC_ELSA_MASK;

}
void LEDbright(char value)
{   LEDON(value,initial);
	if (value == 'i' && initial <1000)
		initial += 100;
	else if(value == 'd' && initial >0)
		initial -= 100;

	LEDON(value,initial);
}
void LEDON(char value, uint16_t intensity)
{
#ifdef MESSAGE
	 if(value ==0x01)
	    {
		TPM2->CONTROLS[0].CnV = intensity;
		TPM2->CONTROLS[1].CnV = 0;
		TPM0->CONTROLS[1].CnV = 0;
		x=1;
		}
		else if(value ==0x02)
		{
		TPM2->CONTROLS[0].CnV = TPM2->CONTROLS[1].CnV = intensity;
		TPM0->CONTROLS[1].CnV = 0;
		x=2;
		}
		else if(value ==0x03)
		{
		TPM2->CONTROLS[1].CnV = intensity;
		TPM2->CONTROLS[0].CnV = 0;
		TPM0->CONTROLS[1].CnV = 0;
		x=3;
		}
		else if(value ==0x03)
		{
		TPM0->CONTROLS[1].CnV = intensity;
		TPM2->CONTROLS[1].CnV = 0;
		TPM2->CONTROLS[0].CnV = 0;
		x=4;
		}
		else if(value ==0x04)
		{
		TPM2->CONTROLS[0].CnV = TPM0->CONTROLS[1].CnV = intensity;
		TPM2->CONTROLS[1].CnV = 0;
		x=5;
		}
		else if(value ==0x05)
		{
		TPM2->CONTROLS[1].CnV = TPM0->CONTROLS[1].CnV = intensity;
		TPM2->CONTROLS[0].CnV = 0;
		x=6;
		}
		else if(value ==0x06)
		{
		TPM2->CONTROLS[1].CnV = TPM2->CONTROLS[0].CnV=TPM0->CONTROLS[1].CnV = intensity;
		x=7;
		}
		else if(value ==(0x09) || value==0x00)
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
#endif
 if(value =='r')
    {
	TPM2->CONTROLS[0].CnV = intensity;
	TPM2->CONTROLS[1].CnV = 0;
	TPM0->CONTROLS[1].CnV = 0;
	x=1;
	}
	else if(value =='y')
	{
	TPM2->CONTROLS[0].CnV = TPM2->CONTROLS[1].CnV = intensity;
	TPM0->CONTROLS[1].CnV = 0;
	x=2;
	}
	else if(value =='g')
	{
	TPM2->CONTROLS[1].CnV = intensity;
	TPM2->CONTROLS[0].CnV = 0;
	TPM0->CONTROLS[1].CnV = 0;
	x=3;
	}
	else if(value =='b')
	{
	TPM0->CONTROLS[1].CnV = intensity;
	TPM2->CONTROLS[1].CnV = 0;
	TPM2->CONTROLS[0].CnV = 0;
	x=4;
	}
	else if(value =='m')
	{
	TPM2->CONTROLS[0].CnV = TPM0->CONTROLS[1].CnV = intensity;
	TPM2->CONTROLS[1].CnV = 0;
	x=5;
	}
	else if(value =='c')
	{
	TPM2->CONTROLS[1].CnV = TPM0->CONTROLS[1].CnV = intensity;
	TPM2->CONTROLS[0].CnV = 0;
	x=6;
	}
	else if(value =='w')
	{
	TPM2->CONTROLS[1].CnV = TPM2->CONTROLS[0].CnV=TPM0->CONTROLS[1].CnV = intensity;
	x=7;
	}
	else if(value ==('i') || value=='d')
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
