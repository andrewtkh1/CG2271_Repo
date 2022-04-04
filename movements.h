
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h"
#include "system_MKL25Z4.h"   	         // Keil::Device:Startup
#include "cmsis_os2.h"


#define TOP_LEFT_WHEEL_FWD	30 					// PORT E pin 30 TM0 CH2 FWD
#define TOP_LEFT_WHEEL_REV	29 					// PORT E pin 29 TM0 CH3 REV
#define BOT_LEFT_WHEEL_FWD 1 						// PORT C 1 Tmp0 Ch0 Forward *CHanged
#define BOT_LEFT_WHEEL_REV 1						// PORT D 1 Tpm0 Ch1 Reverse *CHanged
#define TOP_RIGHT_WHEEL_FWD 8 					// PORT C 8 Tpm0 CHANNEL 4 FWD
#define TOP_RIGHT_WHEEL_REV 9 					// PORT C 9 Tpm0 CHANNEL 5 REV
#define BOT_RIGHT_WHEEL_REV 21					// PORT E 21 TPM1 Ch1 REV
#define BOT_RIGHT_WHEEL_FWD 20					// PORT E 22 Tpm1 Ch0 FWD
#define MASK(x) (1 << (x)) 
/*
* 				TPM2_C0V = Bot left Forward | Change | TPM0_C0V
					TPM2_C1V = Bot left Reverse | Change | TPM0_C1V
					TPM0_C2V = Top left Forward
					TPM0_C3V = Top left Reverse
					TPM0_C4V = Top right Forward
					TPM0_C5V = Top right Reverse
					TPM1_C0V = Bot right Forward
					TPM1_C1V = Bot right Reverse
*/

void initPWM(){
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	
	//Zero out the entire register for PTC8 Pin
	PORTC->PCR[TOP_RIGHT_WHEEL_FWD] &= ~PORT_PCR_MUX_MASK;
	//Set the MUX portion of the PTC8 Pin register to be ALT 3 mode.
	PORTC->PCR[TOP_RIGHT_WHEEL_FWD] |= PORT_PCR_MUX(3);
	
	//Zero out the entire register for PTC9 Pin
	PORTC->PCR[TOP_RIGHT_WHEEL_REV] &= ~PORT_PCR_MUX_MASK;
	//Set the MUX portion of the PTC9 Pin register to be ALT 3 mode.
	PORTC->PCR[TOP_RIGHT_WHEEL_REV] |= PORT_PCR_MUX(3);
	
	PORTC->PCR[BOT_LEFT_WHEEL_FWD] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[BOT_LEFT_WHEEL_FWD] |= PORT_PCR_MUX(4);
	
	PORTE->PCR[BOT_RIGHT_WHEEL_REV] &= ~PORT_PCR_MUX_MASK;	//Clearing the bits of Port E to 0
	PORTE->PCR[BOT_RIGHT_WHEEL_REV] |= PORT_PCR_MUX(3);		//Setting it to be PWM. 3 = PWM.
	
	PORTE->PCR[BOT_RIGHT_WHEEL_FWD] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[BOT_RIGHT_WHEEL_FWD] |= PORT_PCR_MUX(3);
	
	PORTD->PCR[BOT_LEFT_WHEEL_REV] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BOT_LEFT_WHEEL_REV] |= PORT_PCR_MUX(4);
	//enable pin output and set mux TO 3 to use alt 3 for time in port b 01 n 1
	PORTE->PCR[TOP_LEFT_WHEEL_FWD] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[TOP_LEFT_WHEEL_FWD] |= PORT_PCR_MUX(3);
	
	PORTE->PCR[TOP_LEFT_WHEEL_REV] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[TOP_LEFT_WHEEL_REV] |= PORT_PCR_MUX(3);
	
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	// edge aligned pwm
	//ps: 111(128) and CMOD=01
	TPM0->SC &= ~((TPM_SC_CMOD_MASK | (TPM_SC_PS_MASK)));
	TPM0->SC |= (TPM_SC_CMOD(1) | (TPM_SC_PS(7)));
	// set to up counting
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK);
	
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK)); //CMOD = clock mode selection, PS = Prescale Factor Selection
	TPM1->SC |= ((TPM_SC_CMOD(1)) | (TPM_SC_PS(7)));	//PS = 128
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);	//Set PWM to Up counting mode
	
	// Enable PWM on TPM1 channel 2; 0 -> PTB0
	//start from high output first 
	TPM0_C2SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK) );
  TPM0_C2SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	// Enable PWM on TPM1 channel 3; 0 -> PTB1
	TPM0_C3SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK) );
  TPM0_C3SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	TPM0_C0SC &= ~(TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK | TPM_CnSC_MSA_MASK | TPM_CnSC_MSB_MASK);
	TPM0_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	TPM0_C1SC &= ~(TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK | TPM_CnSC_MSA_MASK | TPM_CnSC_MSB_MASK);
	TPM0_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//Set the counter to run as low true Pass for TPM0 Ch 4 & Ch 5
	TPM0_C4SC &= ~((TPM_CnSC_ELSB_MASK | (TPM_CnSC_ELSA_MASK) |(TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK)));
	TPM0_C4SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	TPM0_C5SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK) );
	TPM0_C5SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//For Timer 1 Ch 0 & Ch 1
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));	//Setting whether its count up/down. MSB = Select edge aligned.
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));	//Setting whether its count up/down. MSB = Select edge aligned.
	
	TPM1->MOD = 7500;
	TPM0->MOD = 7500;
	
	TPM0_C0V = 0;
	TPM0_C1V = 0;
	
	TPM0_C2V = 0;
	TPM0_C3V = 0;
	
	TPM0_C4V = 0;
	TPM0_C5V = 0;
	
	TPM1_C0V = 0;
	TPM1_C1V = 0;
}

void stopBot(){
	TPM0_C0V = 0;
	TPM0_C1V = 0;
	TPM0_C2V = 0;
	TPM0_C3V = 0;
	TPM0_C4V = 0;
	TPM0_C5V = 0;
	TPM1_C0V = 0;
	TPM1_C1V = 0;
}
// 0xEA6 = 50% duty cycle.

void forward(){
 	TPM0_C0V = 7500; 	//Bot left Forward
	TPM0_C1V = 0; 			//Bot left Reverse
	TPM0_C2V = 7500;	//Top left Forward
	TPM0_C3V = 0;				//Top left Reverse
	TPM0_C4V = 7500;	//Top right Forward
	TPM0_C5V = 0;				//Top right Reverse
	TPM1_C0V = 7500;	//Bot right Forward
	TPM1_C1V = 0;				//Bot right Reverse
}

void reverse(){
	TPM0_C0V = 0; 			//Bot left Forward
	TPM0_C1V = 7500; 	//Bot left Reverse
	TPM0_C2V = 0;				//Top left Forward
	TPM0_C3V = 7500;	//Top left Reverse
	TPM0_C4V = 0;				//Top right Forward
	TPM0_C5V = 7500;	//Top right Reverse
	TPM1_C0V = 0;				//Bot right Forward
	TPM1_C1V = 7500;	//Bot right Reverse
}

void turnRight(){
	TPM0_C0V = 7500/1.8; 	//Bot left Forward
	TPM0_C1V = 0;				//Bot left Reverse
	TPM0_C2V = 7500/1.8;	//Top left Forward
	TPM0_C3V = 0;				//Top left Reverse
	TPM0_C4V = 0;				//Top right Forward
	TPM0_C5V = 7500/1.8;	//Top right Reverse
	TPM1_C0V = 0;				//Bot right Forward
	TPM1_C1V = 7500/1.8;	//Bot right Reverse
}

void turnLeft(){
	TPM0_C0V = 0; 			//Bot left Forward
	TPM0_C1V = 7500/1.8; 	//Bot left Reverse
	TPM0_C2V = 0;				//Top left Forward
	TPM0_C3V = 7500/1.8;	//Top left Reverse
	TPM0_C4V = 7500/1.8;	//Top right Forward
	TPM0_C5V = 0;				//Top right Reverse
	TPM1_C0V = 7500/1.8;	//Bot right Forward
	TPM1_C1V = 0;				//Bot right Reverse
}

void forwardRight(){
 	TPM0_C0V = 7500; 		//Bot left Forward
	TPM0_C1V = 0; 			//Bot left Reverse
	TPM0_C2V = 7500;		//Top left Forward
	TPM0_C3V = 0;				//Top left Reverse
	TPM0_C4V = 7500/50;	//Top right Forward
	TPM0_C5V = 0;				//Top right Reverse
	TPM1_C0V = 7500/50;	//Bot right Forward
	TPM1_C1V = 0;				//Bot right Reverse
}

void forwardLeft(){
	TPM0_C0V = 7500/100; 	//Bot left Forward
	TPM0_C1V = 0; 				//Bot left Reverse
	TPM0_C2V = 7500/100;		//Top left Forward
	TPM0_C3V = 0;					//Top left Reverse
	TPM0_C4V = 7500;			//Top right Forward
	TPM0_C5V = 0;					//Top right Reverse
	TPM1_C0V = 7500;			//Bot right Forward
	TPM1_C1V = 0;					//Bot right Reverse
}
