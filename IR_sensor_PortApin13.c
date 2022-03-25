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

 volatile unsigned int counter = 0;


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
	
    // on led for x amount of time when obstacle detected
	if(PORTA->ISFR & MASK(IR_SENSOR)) {
		// ON LED on falling edge
		PTB -> PCOR = MASK(RED_LED);
		PTB ->PSOR = (MASK(GREEN_LED));
		PTD ->PSOR = (MASK(BLUE_LED));
		delay(0x80000);
	}
	
	PORTA->ISFR |= MASK(IR_SENSOR);
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
	

int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	
	//initPWM();
	initIRSensor();
	initLED();
	offlled();

    // off led when normal
	while(1) {
		
		PTD ->PSOR = (MASK(BLUE_LED)); 
		PTB ->PSOR = (MASK(RED_LED));
		PTB ->PSOR = (MASK(GREEN_LED));
	}
	

    // polling
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
	
	
	
	
	
 
  //osKernelInitialize();                 // Initialize CMSIS-RTOS
  //osThreadNew(app_main, NULL, NULL);    // Create application main thread
  //osKernelStart();                      // Start thread execution
  //for (;;) {}
}
