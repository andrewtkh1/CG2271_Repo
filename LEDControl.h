#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h" // Device header

#define PTD4_Pin 5 //IR Sensor
#define RED_LED1 5 //PTE5_Pin Pin
#define RED_LED2 4 //PTE4_Pin Pin
#define RED_LED3 3 //PTE3_Pin Pin
#define RED_LED4 2 //PTE2_Pin
#define RED_LED5 11 //PTB11_Pin
#define RED_LED6 10 //PTB10_Pin
#define RED_LED7 9 //PTB9_Pin
#define RED_LED8 8 //PTB8_Pin
#define RED_LED9 1 //PTB1_Pin
#define RED_LED10 0 //PTB0_Pin

#define GREEN_1  7// port C pin 7
#define GREEN_2  0// port C pin 0
#define GREEN_3  3// port C pin 3
#define GREEN_4  4// port C pin 4
#define GREEN_5  5// port C pin 5
#define GREEN_6  6// port C pin 6
#define GREEN_7  10// port C pin 10
#define GREEN_8  11// port C pin 11
#define GREEN_9  12// port C pin 12
#define GREEN_10 13// port C pin 13
#define delayGreen 0x50 // for green strip how fast u wan

#define MASK(x)	(1 << (x))

//1 for true, 0 for false
int isMoving = 0;

void initGreenStrip(void ) {
	//SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK);
	
	PORTC->PCR[GREEN_1] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_2] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_3] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_4] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_5] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_6] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_7] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_8] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_9] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_10] &= ~PORT_PCR_MUX_MASK;
	
	PORTC->PCR[GREEN_1] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_2] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_3] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_4] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_5] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_6] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_7] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_8] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_9] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_10] |= PORT_PCR_MUX(1);
	
	// set as output
	PTC->PDDR |= MASK(GREEN_1) | MASK(GREEN_2) | MASK(GREEN_3) | MASK(GREEN_4) 
							| MASK(GREEN_5) | MASK(GREEN_6) | MASK(GREEN_7) | MASK(GREEN_8) 
							| MASK(GREEN_9) | MASK(GREEN_10) ;

}

void runMode(){
		// on 1
	if (isMoving == 0){
		return;
	}
		PTC ->PSOR = (MASK(GREEN_1)); 
		PTC ->PCOR = (MASK(GREEN_2));
		PTC ->PCOR = (MASK(GREEN_3));
		PTC ->PCOR = (MASK(GREEN_4));
		PTC ->PCOR = (MASK(GREEN_5));
		PTC ->PCOR = (MASK(GREEN_6));
		PTC ->PCOR = (MASK(GREEN_7));
		PTC ->PCOR = (MASK(GREEN_8));
		PTC ->PCOR = (MASK(GREEN_9));
		PTC ->PCOR = (MASK(GREEN_10));
		
		osDelay(delayGreen);
		if (isMoving == 0){
			return;
		}
		// on 2
		PTC ->PCOR = (MASK(GREEN_1)); 
		PTC ->PSOR = (MASK(GREEN_2));
		PTC ->PCOR = (MASK(GREEN_3));
		PTC ->PCOR = (MASK(GREEN_4));
		PTC ->PCOR = (MASK(GREEN_5));
		PTC ->PCOR = (MASK(GREEN_6));
		PTC ->PCOR = (MASK(GREEN_7));
		PTC ->PCOR = (MASK(GREEN_8));
		PTC ->PCOR = (MASK(GREEN_9));
		PTC ->PCOR = (MASK(GREEN_10));
		
		osDelay(delayGreen);
		if (isMoving == 0){
			return;
		}
		PTC ->PCOR = (MASK(GREEN_1)); 
		PTC ->PCOR = (MASK(GREEN_2));
		PTC ->PSOR = (MASK(GREEN_3));
		PTC ->PCOR = (MASK(GREEN_4));
		PTC ->PCOR = (MASK(GREEN_5));
		PTC ->PCOR = (MASK(GREEN_6));
		PTC ->PCOR = (MASK(GREEN_7));
		PTC ->PCOR = (MASK(GREEN_8));
		PTC ->PCOR = (MASK(GREEN_9));
		PTC ->PCOR = (MASK(GREEN_10));
		
		osDelay(delayGreen);
		if (isMoving == 0){
			return;
		}
		PTC ->PCOR = (MASK(GREEN_1)); 
		PTC ->PCOR = (MASK(GREEN_2));
		PTC ->PCOR = (MASK(GREEN_3));
		PTC ->PSOR = (MASK(GREEN_4));
		PTC ->PCOR = (MASK(GREEN_5));
		PTC ->PCOR = (MASK(GREEN_6));
		PTC ->PCOR = (MASK(GREEN_7));
		PTC ->PCOR = (MASK(GREEN_8));
		PTC ->PCOR = (MASK(GREEN_9));
		PTC ->PCOR = (MASK(GREEN_10));
		
		osDelay(delayGreen);
		if (isMoving == 0){
			return;
		}
		PTC ->PCOR = (MASK(GREEN_1)); 
		PTC ->PCOR = (MASK(GREEN_2));
		PTC ->PCOR = (MASK(GREEN_3));
		PTC ->PCOR = (MASK(GREEN_4));
		PTC ->PSOR = (MASK(GREEN_5));
		PTC ->PCOR = (MASK(GREEN_6));
		PTC ->PCOR = (MASK(GREEN_7));
		PTC ->PCOR = (MASK(GREEN_8));
		PTC ->PCOR = (MASK(GREEN_9));
		PTC ->PCOR = (MASK(GREEN_10));
		
		osDelay(delayGreen);
		if (isMoving == 0){
			return;
		}
		PTC ->PCOR = (MASK(GREEN_1)); 
		PTC ->PCOR = (MASK(GREEN_2));
		PTC ->PCOR = (MASK(GREEN_3));
		PTC ->PCOR = (MASK(GREEN_4));
		PTC ->PCOR = (MASK(GREEN_5));
		PTC ->PSOR = (MASK(GREEN_6));
		PTC ->PCOR = (MASK(GREEN_7));
		PTC ->PCOR = (MASK(GREEN_8));
		PTC ->PCOR = (MASK(GREEN_9));
		PTC ->PCOR = (MASK(GREEN_10));
		
		osDelay(delayGreen);
		if (isMoving == 0){
			return;
		}
		PTC ->PCOR = (MASK(GREEN_1)); 
		PTC ->PCOR = (MASK(GREEN_2));
		PTC ->PCOR = (MASK(GREEN_3));
		PTC ->PCOR = (MASK(GREEN_4));
		PTC ->PCOR = (MASK(GREEN_5));
		PTC ->PCOR = (MASK(GREEN_6));
		PTC ->PSOR = (MASK(GREEN_7));
		PTC ->PCOR = (MASK(GREEN_8));
		PTC ->PCOR = (MASK(GREEN_9));
		PTC ->PCOR = (MASK(GREEN_10));
		
		osDelay(delayGreen);
		if (isMoving == 0){
			return;
		}
		PTC ->PCOR = (MASK(GREEN_1)); 
		PTC ->PCOR = (MASK(GREEN_2));
		PTC ->PCOR = (MASK(GREEN_3));
		PTC ->PCOR = (MASK(GREEN_4));
		PTC ->PCOR = (MASK(GREEN_5));
		PTC ->PCOR = (MASK(GREEN_6));
		PTC ->PCOR = (MASK(GREEN_7));
		PTC ->PSOR = (MASK(GREEN_8));
		PTC ->PCOR = (MASK(GREEN_9));
		PTC ->PCOR = (MASK(GREEN_10));
		
		osDelay(delayGreen);
		if (isMoving == 0){
			return;
		}
		PTC ->PCOR = (MASK(GREEN_1)); 
		PTC ->PCOR = (MASK(GREEN_2));
		PTC ->PCOR = (MASK(GREEN_3));
		PTC ->PCOR = (MASK(GREEN_4));
		PTC ->PCOR = (MASK(GREEN_5));
		PTC ->PCOR = (MASK(GREEN_6));
		PTC ->PCOR = (MASK(GREEN_7));
		PTC ->PCOR = (MASK(GREEN_8));
		PTC ->PSOR = (MASK(GREEN_9));
		PTC ->PCOR = (MASK(GREEN_10));
		
		osDelay(delayGreen);
		if (isMoving == 0){
			return;
		}
		PTC ->PCOR = (MASK(GREEN_1)); 
		PTC ->PCOR = (MASK(GREEN_2));
		PTC ->PCOR = (MASK(GREEN_3));
		PTC ->PCOR = (MASK(GREEN_4));
		PTC ->PCOR = (MASK(GREEN_5));
		PTC ->PCOR = (MASK(GREEN_6));
		PTC ->PCOR = (MASK(GREEN_7));
		PTC ->PCOR = (MASK(GREEN_8));
		PTC ->PCOR = (MASK(GREEN_9));
		PTC ->PSOR = (MASK(GREEN_10));
		osDelay(delayGreen);
}

void stationaryMode() {
	
		PTC ->PSOR = (MASK(GREEN_1)); 
		PTC ->PSOR = (MASK(GREEN_2));
		PTC ->PSOR = (MASK(GREEN_3));
		PTC ->PSOR = (MASK(GREEN_4));
		PTC ->PSOR = (MASK(GREEN_5));
		PTC ->PSOR = (MASK(GREEN_6));
		PTC ->PSOR = (MASK(GREEN_7));
		PTC ->PSOR = (MASK(GREEN_8));
		PTC ->PSOR = (MASK(GREEN_9));
		PTC ->PSOR = (MASK(GREEN_10));
		osDelay(delayGreen);
}

void initRedLED(){	
	PORTE->PCR[RED_LED1] &= ~PORT_PCR_MUX(1);
	PORTE->PCR[RED_LED1] |= PORT_PCR_MUX(1);
	
	PORTE->PCR[RED_LED2] &= ~PORT_PCR_MUX(1);
	PORTE->PCR[RED_LED2] |= PORT_PCR_MUX(1);

	PORTE->PCR[RED_LED3] &= ~PORT_PCR_MUX(1);
	PORTE->PCR[RED_LED3] |= PORT_PCR_MUX(1);

	PORTE->PCR[RED_LED4] &= ~PORT_PCR_MUX(1);
	PORTE->PCR[RED_LED4] |= PORT_PCR_MUX(1);

	PORTB->PCR[RED_LED5] &= ~PORT_PCR_MUX(1);
	PORTB->PCR[RED_LED5] |= PORT_PCR_MUX(1);

	PORTB->PCR[RED_LED6] &= ~PORT_PCR_MUX(1);
	PORTB->PCR[RED_LED6] |= PORT_PCR_MUX(1);

	PORTB->PCR[RED_LED7] &= ~PORT_PCR_MUX(1);
	PORTB->PCR[RED_LED7] |= PORT_PCR_MUX(1);

	PORTB->PCR[RED_LED8] &= ~PORT_PCR_MUX(1);
	PORTB->PCR[RED_LED8] |= PORT_PCR_MUX(1);

	PORTB->PCR[RED_LED9] &= ~PORT_PCR_MUX(1);
	PORTB->PCR[RED_LED9] |= PORT_PCR_MUX(1);

	PORTB->PCR[RED_LED10] &= ~PORT_PCR_MUX(1);
	PORTB->PCR[RED_LED10] |= PORT_PCR_MUX(1);


	//Set the Direction of the Pins
	PTB->PDDR |= (MASK(RED_LED5) | MASK(RED_LED6) | MASK(RED_LED7) | MASK(RED_LED8)| MASK(RED_LED9)| MASK(RED_LED10)) ;
	//PTB->PDDR |= MASK(RED_LED8);
	PTE->PDDR |= (MASK(RED_LED1) | MASK(RED_LED2) | MASK(RED_LED3) | MASK(RED_LED4));
}

void blinkGreenLED(void *argument){
	while(1){
		if (isMoving == 1){
			runMode();
		} else {
			stationaryMode();
		}
	}
}

//Thread for Red LED
void blinkRedLED(void *argument){
	while(1){
		//Turn on the RED LEDs
		PTB->PSOR = (MASK(RED_LED5) | MASK(RED_LED6) | MASK(RED_LED7) | MASK(RED_LED8)| MASK(RED_LED9)| MASK(RED_LED10));
		PTE->PSOR = (MASK(RED_LED1) | MASK(RED_LED2) | MASK(RED_LED3) | MASK(RED_LED4));
	
		if(isMoving == 1){
			osDelay(500);
		}else{
			osDelay(250);
		}
		//Turn off the RED LEDs
		PTB->PCOR = (MASK(RED_LED5) | MASK(RED_LED6) | MASK(RED_LED7) | MASK(RED_LED8)| MASK(RED_LED9)| MASK(RED_LED10));
		PTE->PCOR = (MASK(RED_LED1) | MASK(RED_LED2) | MASK(RED_LED3) | MASK(RED_LED4));
	
		if(isMoving == 1){
			osDelay(500);
		}else{
			osDelay(250);
		}
	}
}

