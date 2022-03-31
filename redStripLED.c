/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h" // Device header
#define PTA5_Pin 5 //CHANNEL 2
#define PTA4_Pin 4 //CHANNEL 1
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
#define MASK(x)	(1 << (x))

//1 for true, 0 for false
int isMoving = 0;
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
 
  // ...
  for (;;) {}
}

void initRedLED(){
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	//Set the pins to GPIO
	
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

void blinkRedLED(){
	while(1){
		//Turn on the RED LEDs
		PTB->PSOR |= (MASK(RED_LED5) | MASK(RED_LED6) | MASK(RED_LED7) | MASK(RED_LED8)| MASK(RED_LED9)| MASK(RED_LED10));
		PTE->PSOR |= (MASK(RED_LED1) | MASK(RED_LED2) | MASK(RED_LED3) | MASK(RED_LED4));
	
		if(isMoving == 1){
			osDelay(500);
		}else{
			osDelay(250);
		}
		//Turn off the RED LEDs
		PTB->PCOR |= (MASK(RED_LED5) | MASK(RED_LED6) | MASK(RED_LED7) | MASK(RED_LED8)| MASK(RED_LED9)| MASK(RED_LED10));
		PTE->PCOR |= (MASK(RED_LED1) | MASK(RED_LED2) | MASK(RED_LED3) | MASK(RED_LED4));
	
		if(isMoving == 1){
			osDelay(500);
		}else{
			osDelay(250);
		}
	}
}

int main (void) {
  SystemCoreClockUpdate();
  initRedLED();
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(blinkRedLED, NULL, NULL);    // Create application main thread
  osKernelStart();   	// Start thread execution
  for (;;) {}
}
