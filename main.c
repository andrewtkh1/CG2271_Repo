/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h"
#include "system_MKL25Z4.h"   	         // Keil::Device:Startup
#include "cmsis_os2.h"

//Variables for interrupt
//#define BAUD_RATE 115200
#define BAUD_RATE 9600
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 64
volatile uint16_t rx_data;

#define TOP_LEFT_WHEEL_FWD	30 					// PORT E pin 30 TM0 CH2 FWD
#define TOP_LEFT_WHEEL_REV	29 					// PORT E pin 29 TM0 CH3 REV
#define BOT_LEFT_WHEEL_FWD 22 					// Tmp2 Ch0 Forward
#define BOT_LEFT_WHEEL_REV 2						// Tpm2 Ch1 Reverse
#define TOP_RIGHT_WHEEL_FWD 8 					// Tpm0 CHANNEL 4 FWD
#define TOP_RIGHT_WHEEL_REV 9 					// Tpm0 CHANNEL 5 REV
#define BOT_RIGHT_WHEEL_REV 21					// TPM1 Ch1 REV
#define BOT_RIGHT_WHEEL_FWD 20					// Tpm1 Ch0 FWD
#define MASK(x) (1 << (x)) 

void initUART2Interrupt(){
    //Enable RIE so that when we are ready to receive, the interrupt would also be triggered and we can do some action.

    //Enable interrupt for USART2 so that the interrupt can be triggered.
    NVIC_EnableIRQ(UART2_IRQn);
}

void initUART2(uint32_t baud_rate)
{
	uint32_t divisor, bus_clock;
	
	//enable clock to uart2 and porte
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	//connect UART pins for PTE23

	PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);
	
	//Ensure Rx is disable before configuration
	UART2->C2 &= ~(UART_C2_RE_MASK);
	
	// set baud rate to desired value
	bus_clock = (DEFAULT_SYSTEM_CLOCK)/2;
	divisor = bus_clock / (baud_rate * 16);
	UART2->BDH = UART_BDH_SBR(divisor >> 8);
	UART2->BDH |= UART_BDH_SBNS(0); // SHould be set stop bit to 1
	UART2->BDL = UART_BDL_SBR(divisor);
	
	// set piroity fot interrupt uart
	NVIC_SetPriority(UART2_IRQn, UART2_INT_PRIO);
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);
	UART2->C2 |= UART_C2_RIE_MASK;

	initUART2Interrupt();
	//No pairity, 8 bits
	UART2->C1 = 0;
	UART2->S2 = 0;
	UART2->C3 = 0;
	
	// enable RX
	UART2->C2 |= (UART_C2_RE_MASK);
}

void initPWM(){
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM_SCGC6 |= SIM_SCGC6_TPM2_MASK;
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
	
	PORTE->PCR[BOT_LEFT_WHEEL_FWD] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[BOT_LEFT_WHEEL_FWD] |= PORT_PCR_MUX(3);
	
	PORTE->PCR[BOT_RIGHT_WHEEL_REV] &= ~PORT_PCR_MUX_MASK;	//Clearing the bits of Port E to 0
	PORTE->PCR[BOT_RIGHT_WHEEL_REV] |= PORT_PCR_MUX(3);		//Setting it to be PWM. 3 = PWM.
	
	PORTE->PCR[BOT_RIGHT_WHEEL_FWD] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[BOT_RIGHT_WHEEL_FWD] |= PORT_PCR_MUX(3);
	
	PORTA->PCR[BOT_LEFT_WHEEL_REV] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[BOT_LEFT_WHEEL_REV] |= PORT_PCR_MUX(3);
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
	
	TPM2->SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK);
	TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM2->SC &= ~TPM_SC_CPWMS_MASK;
	
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
	
	TPM2_C0SC &= ~(TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK | TPM_CnSC_MSA_MASK | TPM_CnSC_MSB_MASK);
	TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	TPM2_C1SC &= ~(TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK | TPM_CnSC_MSA_MASK | TPM_CnSC_MSB_MASK);
	TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
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
	
	TPM2->MOD = 7500;
	TPM1->MOD = 7500;
	TPM0->MOD = 7500;
}

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/

/*
* 				TPM2_C0V = Bot left Forward
					TPM2_C1V = Bot left Reverse
					TPM0_C2V = Top left Forward
					TPM0_C3V = Top left Reverse
					TPM0_C4V = Top right Forward
					TPM0_C5V = Top right Reverse
					TPM1_C0V = Bot right Forward
					TPM1_C1V = Bot right Reverse
*/

void stopBot(){
	TPM2_C0V = 0;
	TPM2_C1V = 0;
	TPM0_C2V = 0;
	TPM0_C3V = 0;
	TPM0_C4V = 0;
	TPM0_C5V = 0;
	TPM1_C0V = 0;
	TPM1_C1V = 0;
}

void forward(){
 	TPM2_C0V = 0x0EA6; 	//Bot left Forward
	TPM2_C1V = 0; 			//Bot left Reverse
	TPM0_C2V = 0x0EA6;	//Top left Forward
	TPM0_C3V = 0;				//Top left Reverse
	TPM0_C4V = 0x0EA6;	//Top right Forward
	TPM0_C5V = 0;				//Top right Reverse
	TPM1_C0V = 0x0EA6;	//Bot right Forward
	TPM1_C1V = 0;				//Bot right Reverse
}

void reverse(){
	TPM2_C0V = 0; 			//Bot left Forward
	TPM2_C1V = 0x0EA6; 	//Bot left Reverse
	TPM0_C2V = 0;				//Top left Forward
	TPM0_C3V = 0x0EA6;	//Top left Reverse
	TPM0_C4V = 0;				//Top right Forward
	TPM0_C5V = 0x0EA6;	//Top right Reverse
	TPM1_C0V = 0;				//Bot right Forward
	TPM1_C1V = 0x0EA6;	//Bot right Reverse
}

void turnRight(){
	TPM2_C0V = 0x0EA6; 	//Bot left Forward
	TPM2_C1V = 0;				//Bot left Reverse
	TPM0_C2V = 0x0EA6;	//Top left Forward
	TPM0_C3V = 0;				//Top left Reverse
	TPM0_C4V = 0;				//Top right Forward
	TPM0_C5V = 0x0EA6;	//Top right Reverse
	TPM1_C0V = 0;				//Bot right Forward
	TPM1_C1V = 0x0EA6;	//Bot right Reverse
}

void turnLeft(){
	TPM2_C0V = 0; 			//Bot left Forward
	TPM2_C1V = 0x0EA6; 	//Bot left Reverse
	TPM0_C2V = 0;				//Top left Forward
	TPM0_C3V = 0x0EA6;	//Top left Reverse
	TPM0_C4V = 0x0EA6;	//Top right Forward
	TPM0_C5V = 0;				//Top right Reverse
	TPM1_C0V = 0x0EA6;	//Bot right Forward
	TPM1_C1V = 0;				//Bot right Reverse
}

//UART2 ISR
void UART2_IRQHandler(void){
	   if (UART2->S1 & UART_S1_RDRF_MASK) {
			rx_data = UART2->D;
		 }
		 
		 if (UART2->S1 & UART_S1_TDRE_MASK) {
			//UART2->D = 0;
		 }
		 
		 		// handle the error
				// clear the flag
		 if (UART2->S1 & (UART_S1_OR_MASK |
			UART_S1_NF_MASK | 
			UART_S1_FE_MASK | 
			UART_S1_PF_MASK)) {
				int s = 0;
			}

    //NVIC_ClearPendingIRQ(UART2_IRQn);
    //We need to check which one triggered the ISR as send and receive both triggers the same ISR.
    //Check to see if read was the one that triggered the ISR.
    
		//NVIC_EnableIRQ(UART2_IRQn);
}

void movement (void *argument) {
	//To implement mutext or Queue to not waste CPU cycles.
	for(;;){
		if (rx_data == 0x0b) {
			stopBot();
		} else if (rx_data == 0x1) {
			forward();
			//osDelay(1000);
			//rx_data = 0;
		} else if (rx_data == 0x2) {
			reverse();
			//osDelay(1000);
			//rx_data = 0;
		} else if (rx_data == 0x3) {
			turnLeft();
			//osDelay(1000);
			//rx_data = 0;
		} else if (rx_data == 0x4) {
			turnRight();
			//osDelay(1000);
			//rx_data = 0;
		} else {
			stopBot();
		}
	}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initPWM();
	initUART2(BAUD_RATE);
	
	int flag = 0;
	TPM2_C0V = 0;
	TPM2_C1V = 0;
	
	TPM0_C2V = 0;
	TPM0_C3V = 0;
	
	TPM0_C4V = 0;
	TPM0_C5V = 0;
	
	TPM1_C0V = 0;
	TPM1_C1V = 0;	
  // ...
	
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  //osThreadNew(app_main, NULL, NULL);   // Create application main thread
	osThreadNew(movement, NULL, NULL); 		//Thread for movements
  osKernelStart();                      // Start thread execution
  //int flag = 0;
	for (;;) {
	}
}
