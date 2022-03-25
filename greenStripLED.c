/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
#include "MKL25Z4.h" 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#define MASK(x) (1 << (x))
#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1

#define TOP_LEFT_WHEEL_1	30 // PORT E pin 30 CH2
#define TOP_LEFT_WHEEL_2	29 // PORT E pin 29 CH3
#define IR_SENSOR 				13 // PORT A pin 1

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
#define delayGreen 0x100 // for green strip how fast u wan


 volatile unsigned int counter = 0;
 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
 
  // ...
  for (;;) {}
}

void initIRSensor(void)
{
	// enabile clock for port A
	SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK);
	
	// configure mux to GPIO
	PORTA->PCR[IR_SENSOR] &= ~PORT_PCR_MUX_MASK;
	// configure at falling edge
	PORTA->PCR[IR_SENSOR] |= (PORT_PCR_MUX(1) | PORT_PCR_IRQC(0B1000));
	//PORTE->PCR[IR_SENSOR] |= PORT_PCR_MUX(1);
	
	// Set data direction registor for port A as input
	PTA->PDDR &= ~MASK(IR_SENSOR);
	
	// enable interupts
	NVIC_SetPriority(PORTA_IRQn, 1);
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTA_IRQn);
}

 
/* Delay Function */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
	}

}

void PORTA_IRQHandler() {
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	
	counter = 1;
	
	if(PORTA->ISFR & MASK(IR_SENSOR)) {
		// ON LED on falling edge
		PTB -> PCOR = MASK(RED_LED);
		PTB ->PSOR = (MASK(GREEN_LED));
		PTD ->PSOR = (MASK(BLUE_LED));
		delay(0x80000);
	}
	
	PORTA->ISFR |= MASK(IR_SENSOR);
}

void initGreenStrip(void ) {
	SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK);
	
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

void initLED (void) {
	// Enable Clock to PORTB and PORTD
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
	
	// Configure MUX settings to make all 3 pins GPIO
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
	
	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
	
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
	
	// Set Data Direction Registers for PortB and PortD
	PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PDDR |= MASK(BLUE_LED);
}

void offlled(void){
	PTB ->PSOR = (MASK(RED_LED) | MASK(GREEN_LED));
	PTD ->PSOR = (MASK(BLUE_LED));
}

void stationaryMode(void *argument) {
	
	for(;;) {
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
}



void runMode(void *argument){
	
	for(;;){
		// on 1
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
}
	


int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initGreenStrip();
	
	// on all when stationary
	//stationaryMode();
	//delay(0x800000);
	

	
	osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(runMode, NULL, NULL);    // Create application main thread
	osThreadNew(stationaryMode, NULL, NULL);
  osKernelStart();                      // Start thread execution
  for (;;) {
		
	}
	
	
	//while (1) 
	//{
		/*if(PTE -> PDIR & MASK(IR_SENSOR)) {
			PTB -> PCOR = MASK(RED_LED);
			PTB ->PSOR = (MASK(GREEN_LED));
			PTD ->PSOR = (MASK(BLUE_LED));
		} else {
			PTD ->PSOR = (MASK(BLUE_LED)); 
			PTB ->PSOR = (MASK(RED_LED));
			PTB ->PSOR = (MASK(GREEN_LED));
		}*/
		
		
			
	//}
	
	

	
	
	
	
	//TPM0_C2V = 0x0EA6;
	//TPM0_C2V = 0x0EA6;
	//delay(0x40000);
	
	
	/**
	// Configure MUX settings to make all 3 pins GPIO
	PORTE->PCR[TOP_LEFT_WHEEL_1] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[TOP_LEFT_WHEEL_1] |= PORT_PCR_MUX(1);
	
	
	// Set Data Direction Registers for PortB and PortD
	PTE->PDDR |= (MASK(TOP_LEFT_WHEEL_1) );
	PTE ->PCOR = MASK(TOP_LEFT_WHEEL_1);
	
	delay(0x800000);
	
	//enable pin output and set mux TO 3 to use alt 3 for time in port b 01 n 1
	PORTE->PCR[TOP_LEFT_WHEEL_1] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[TOP_LEFT_WHEEL_1] |= PORT_PCR_MUX(3);
	**/
	
	
	
	
	
 
  
}
