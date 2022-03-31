
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h" // Device header
#define PTA5_Pin 5 //CHANNEL 2
#define PTA4_Pin 4 //CHANNEL 1
#define PTD5_Pin 5 //IR Sensor
#define RED_LED	 18 //PortB Pin 18 LED
#define MASK(x)	(1 << (x))


void initLED() {
	//Enable Clock to PORTB
	SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK);
	//The first code clears the MUX bit position. 
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	// This one sets the PCR MUX part to set the RED_LED to be a GPIO PIN.
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
	PTB->PDDR |= MASK(RED_LED);
}

void initIRSensor() {
	//Enable clock to port E
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	//Configure Mux for PortE pin 4
	PORTD->PCR[PTD5_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[PTD5_Pin] |= PORT_PCR_MUX(1);
	//Set PDDR to input for PortE pin4.
	PTD->PDDR &= ~MASK(PTD5_Pin);
}

static void delay(volatile uint32_t nof) {
	while (nof != 0) {
		__asm("NOP");
		nof--;
	}
}

void PORTD_IRQHandler() {
	if (PORTD->ISFR & MASK(PTD5_Pin)) {
		//PTE output high when item not detected.
		PTB->PCOR |= MASK(RED_LED);
		delay(0X800000);
	}
	PORTD->ISFR |= MASK(RED_LED);
}


void initIRSensorInterrupt() {
	PORTD->PCR[PTD5_Pin] |= PORT_PCR_IRQC(0b1000);
	NVIC_SetPriority(PORTD_IRQn, 1);
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);
}


int main(void) {
	// System Initialization
	SystemCoreClockUpdate();
	initLED();
	initIRSensor();
	initIRSensorInterrupt();
	for (;;) {
		PTB->PSOR |= MASK(RED_LED);
	}
}