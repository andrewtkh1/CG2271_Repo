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

#define TRIGGER 12 // PORTC PIN 12
#define ECHO 13 // PORTC PIN 13

int volatile counter;
int volatile wait_delay;
int volatile duration = 10000;


void init_pit(){
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[TRIGGER] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[TRIGGER] |= PORT_PCR_MUX(1);
	PORTC->PCR[ECHO] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[ECHO] |= PORT_PCR_MUX(1);
	
	PTC->PDDR |= MASK(TRIGGER);
	PTC->PDDR &= ~MASK(ECHO);
	
	
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT->MCR = 0;
	
	PIT->CHANNEL[0].LDVAL = 47;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;
	
	NVIC_SetPriority(PIT_IRQn,2);
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn);
}

void PIT_IRQHandler(){
	if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK){
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		counter--;
		if(counter == 0){
			PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
			wait_delay = 0;
		}
	}
}


int detect(void){
	PTC->PDOR &= ~MASK(TRIGGER);
	wait_delay = 1;
	counter = 2;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	while(wait_delay);
	
	PTC->PDOR |= MASK(TRIGGER);
	wait_delay = 1;
	counter = 10;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	while(wait_delay);
	
	PTC->PDOR &= ~MASK(TRIGGER);
	
	wait_delay = 1;
	counter = 10000;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	while ((PTC->PDIR & MASK(ECHO)) == 0){
		if(!wait_delay){
			duration = 200000;
			return -1;
		}
	}
	while ((PTC->PDIR & MASK(ECHO)) == MASK(ECHO)){
		if(!wait_delay){
			duration = 200000;
			return -1;
		}
	}
	
	duration = 10000 - counter;
	
	if(duration <=1000)
		return 1;
	else return 0;

}

typedef enum led_colors
{
	red = RED_LED, green = GREEN_LED, blue = BLUE_LED
}led_colors_t;



void InitGPIO(void)
{
	// Enable Clock to PORTB and PORTD
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
	
	// Configure MUX settings to make all 3 pins GPIO
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
	
	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
	
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
	
	//PORTA->PCR[VCC] &= ~PORT_PCR_MUX_MASK;
	//PORTA->PCR[VCC] |= PORT_PCR_MUX(1);
	
	// Set Data Direction Registers for PortB and PortD
	PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PDDR |= MASK(BLUE_LED);
	
	//PTA->PDDR |= MASK(VCC);
}


void offlled(void){
	PTB ->PSOR = (MASK(RED_LED) | MASK(GREEN_LED));
	PTD ->PSOR = (MASK(BLUE_LED));
}



int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	InitGPIO();
	init_pit();

	while(1) {
		if(detect()){
				PTB -> PCOR = MASK(RED_LED);
				PTB ->PSOR = (MASK(GREEN_LED));
				PTD ->PSOR = (MASK(BLUE_LED));
		} else {
				PTB -> PSOR = MASK(RED_LED);
				PTB ->PSOR = (MASK(GREEN_LED));
				PTD ->PSOR = (MASK(BLUE_LED));
		}
}
		
		
	//initGreenStrip();
	//osThreadNew(thread_buzzer, NULL, NULL);
	//osKernelStart();                      		// Start thread execution
		
}


	
	
	
	
	
 
  

