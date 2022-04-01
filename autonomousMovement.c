/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h"
#include "system_MKL25Z4.h"   	         // Keil::Device:Startup
#include "cmsis_os2.h"
//Semaphore Global ID
osSemaphoreId_t autoSelect;
//Variables for interrupt IR sensor
#define IR_SENSOR_1	13 //PortA 13
#define IR_SENSOR_2 5	//PortD 5
#define PORTA_INT_PRIO 1

#define TOP_LEFT_WHEEL_FWD	30 					// PORT E pin 30 TM0 CH2 FWD
#define TOP_LEFT_WHEEL_REV	29 					// PORT E pin 29 TM0 CH3 REV
#define BOT_LEFT_WHEEL_FWD 22 					// Tmp2 Ch0 Forward
#define BOT_LEFT_WHEEL_REV 2						// Tpm2 Ch1 Reverse
#define TOP_RIGHT_WHEEL_FWD 8 					// Tpm0 CHANNEL 4 FWD
#define TOP_RIGHT_WHEEL_REV 9 					// Tpm0 CHANNEL 5 REV
#define BOT_RIGHT_WHEEL_REV 21					// TPM1 Ch1 REV
#define BOT_RIGHT_WHEEL_FWD 20					// Tpm1 Ch0 FWD
#define MASK(x) (1 << (x)) 

volatile int front_Sensor_Trigger = -1;
volatile int back_Sensor_Trigger = 0;

void PORTA_IRQHandler() {

	NVIC_ClearPendingIRQ(PORTA_IRQn);
	
    // on led for x amount of time when obstacle detected
	if(PORTA->ISFR & MASK(IR_SENSOR_1)) {
		// Set the flag for Front IR trigger
		front_Sensor_Trigger += 1;
	}
	
	PORTA->ISFR |= MASK(IR_SENSOR_1);
	NVIC_DisableIRQ(PORTA_IRQn);
}

void PORTD_IRQHandler() {
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	if (PORTD->ISFR & MASK(IR_SENSOR_2)) {
		//PTE output high when item not detected.
		back_Sensor_Trigger = 1;
	}
	PORTD->ISFR |= MASK(IR_SENSOR_2);
}

void initIRSensor(void)
{
	//Enable clock for Port A
	SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK);
	//Enable clock to port D
	SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK);
	//Configure mux to GPIO
	PORTA->PCR[IR_SENSOR_1] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[IR_SENSOR_2] &= ~PORT_PCR_MUX_MASK;
	//Configure to activate at PortD=Rising edge, PortA=logic 0
	PORTA->PCR[IR_SENSOR_1] |= (PORT_PCR_MUX(1) | PORT_PCR_IRQC(0B1000));
	PORTD->PCR[IR_SENSOR_2] |= (PORT_PCR_MUX(1) | PORT_PCR_IRQC(0B1001));
	
	//Set PortA PDDR as Input
	PTA->PDDR &= ~MASK(IR_SENSOR_1);
	PTD->PDDR &= ~MASK(IR_SENSOR_2);
	//Enable Interrupts
	NVIC_SetPriority(PORTA_IRQn, 1);
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_SetPriority(PORTD_IRQn, 1);
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	//NVIC_EnableIRQ(PORTD_IRQn);
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
// 0xEA6 = 50% duty cycle.

void forward(){
 	TPM2_C0V = 7500; 	//Bot left Forward
	TPM2_C1V = 0; 			//Bot left Reverse
	TPM0_C2V = 7500;	//Top left Forward
	TPM0_C3V = 0;				//Top left Reverse
	TPM0_C4V = 7500;	//Top right Forward
	TPM0_C5V = 0;				//Top right Reverse
	TPM1_C0V = 7500;	//Bot right Forward
	TPM1_C1V = 0;				//Bot right Reverse
}

void reverse(){
	TPM2_C0V = 0; 			//Bot left Forward
	TPM2_C1V = 7500; 	//Bot left Reverse
	TPM0_C2V = 0;				//Top left Forward
	TPM0_C3V = 7500;	//Top left Reverse
	TPM0_C4V = 0;				//Top right Forward
	TPM0_C5V = 7500;	//Top right Reverse
	TPM1_C0V = 0;				//Bot right Forward
	TPM1_C1V = 7500;	//Bot right Reverse
}

void turnRight(){
	TPM2_C0V = 7500/2; 	//Bot left Forward
	TPM2_C1V = 0;				//Bot left Reverse
	TPM0_C2V = 7500/2;	//Top left Forward
	TPM0_C3V = 0;				//Top left Reverse
	TPM0_C4V = 0;				//Top right Forward
	TPM0_C5V = 7500/2;	//Top right Reverse
	TPM1_C0V = 0;				//Bot right Forward
	TPM1_C1V = 7500/2;	//Bot right Reverse
}

void turnLeft(){
	TPM2_C0V = 0; 			//Bot left Forward
	TPM2_C1V = 7500/2; 	//Bot left Reverse
	TPM0_C2V = 0;				//Top left Forward
	TPM0_C3V = 7500/2;	//Top left Reverse
	TPM0_C4V = 7500/2;	//Top right Forward
	TPM0_C5V = 0;				//Top right Reverse
	TPM1_C0V = 7500/2;	//Bot right Forward
	TPM1_C1V = 0;				//Bot right Reverse
}

void forwardRight(){
 	TPM2_C0V = 7500; 	//Bot left Forward
	TPM2_C1V = 0; 			//Bot left Reverse
	TPM0_C2V = 7500;	//Top left Forward
	TPM0_C3V = 0;				//Top left Reverse
	TPM0_C4V = 7500/10;//Top right Forward
	TPM0_C5V = 0;				//Top right Reverse
	TPM1_C0V = 7500/10;//Bot right Forward
	TPM1_C1V = 0;				//Bot right Reverse
}

void forwardLeft(){
	TPM2_C0V = 7500/10; 	//Bot left Forward
	TPM2_C1V = 0; 				//Bot left Reverse
	TPM0_C2V = 7500/10;	//Top left Forward
	TPM0_C3V = 0;					//Top left Reverse
	TPM0_C4V = 7500;		//Top right Forward
	TPM0_C5V = 0;					//Top right Reverse
	TPM1_C0V = 7500;		//Bot right Forward
	TPM1_C1V = 0;					//Bot right Reverse
}


void movement (void *argument) {
	//To implement mutext or Queue to not waste CPU cycles.
	for(;;){
		osSemaphoreAcquire(autoSelect, osWaitForever);
		osDelay(5000);
		osSemaphoreRelease(autoSelect);
	}
}

void autonomousMovement(void *argument) {
	osSemaphoreAcquire(autoSelect, osWaitForever);
	initIRSensor();
	NVIC_EnableIRQ(PORTA_IRQn);
	for(;;) {
		//Move forward for a certain distance until Front_IR Triggers
		forward();
		if(front_Sensor_Trigger >= 1) {
		// When IR_Sensor goes Low.
			//NVIC_DisableIRQ(PORTA_IRQn);
			stopBot();
			//turnLeft();
			//osDelay(1000);
			stopBot();
			while(1){};
			//front_Sensor_Trigger = 0;
		}
		if(back_Sensor_Trigger == 1) {
				// Whne Back IR_Sensor goes Low
				stopBot();
				turnRight();
				osDelay(1000);
				stopBot();
				back_Sensor_Trigger = 0;
			}
		//Stop Button for E-Stop
		/*if (rx_data == 0x0b) {
			stopBot();
		}*/
	}
}

 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initPWM();
	
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
	autoSelect = osSemaphoreNew(1,1,NULL);
  //osThreadNew(app_main, NULL, NULL);   // Create application main thread
	//osThreadNew(movement, NULL, NULL); 		//Thread for movements
	osThreadNew(autonomousMovement,NULL,NULL);
  osKernelStart();                      // Start thread execution
  //int flag = 0;
	for (;;) {
	}
}
