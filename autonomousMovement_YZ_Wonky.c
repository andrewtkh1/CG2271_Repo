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
#define TRIGGER 12 // PORTC PIN 12 Ultrasonic Trigger
#define ECHO 13 // PORTC PIN 13 Ultrasonic Trigger
#define PORTA_INT_PRIO 1

#define TOP_LEFT_WHEEL_FWD	30 					// PORT E pin 30 TM0 CH2 FWD
#define TOP_LEFT_WHEEL_REV	29 					// PORT E pin 29 TM0 CH3 REV
#define BOT_LEFT_WHEEL_FWD 1 						// PORT C 1 Tmp0 Ch0 Forward *CHanged
#define BOT_LEFT_WHEEL_REV 1						// PORT D 1 Tpm0 Ch1 Reverse *CHanged
#define TOP_RIGHT_WHEEL_FWD 8 					// Tpm0 CHANNEL 4 FWD
#define TOP_RIGHT_WHEEL_REV 9 					// Tpm0 CHANNEL 5 REV
#define BOT_RIGHT_WHEEL_REV 21					// TPM1 Ch1 REV
#define BOT_RIGHT_WHEEL_FWD 20					// Tpm1 Ch0 FWD
#define GREEN_1  7// port C pin 7
#define GREEN_LED 7 // PORT D Pin 7
#define MASK(x) (1 << (x)) 

volatile int front_Sensor_Trigger = -1;
//volatile int portA_Handler_Counter = -1;
volatile int back_Sensor_Trigger = 0;
//volatile int portD_Handler_Counter = -1;
//1 means the robot has went round the obstacle.
int went_round_obstacle = 0;

int volatile counter;
int volatile wait_delay;
int volatile duration = 10000;
int volatile isdetect;


volatile int triggered = 0;

void initGreenPin (){
	//SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
	PTD->PDDR |= MASK(GREEN_LED);
}


void initGreenStrip(void ) {
	//SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK);
	
	PORTC->PCR[GREEN_1] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_1] |= PORT_PCR_MUX(1);
	
	// set as output
	PTC->PDDR |= MASK(GREEN_1) ;

}


//The PIT Interrupt for Ultrasonic sensor. Adapted from https://github.com/judowha/cg2271_project/blob/master/main.c
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

//Adapted from https://github.com/judowha/cg2271_project/blob/master/main.c
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

//Adapted from https://github.com/judowha/cg2271_project/blob/master/main.c
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
	
	//Change here
	if(duration <=2400)
		return 1;
	else return 0;

}



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

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
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
 	TPM0_C0V = 7050; 	//Bot left Forward
	TPM0_C1V = 0; 			//Bot left Reverse
	TPM0_C2V = 7050;	//Top left Forward
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

void turnRightAuto(){
	TPM0_C0V = 7500/1.2; 	//Bot left Forward
	TPM0_C1V = 0;				//Bot left Reverse
	TPM0_C2V = 7500/1.2;	//Top left Forward
	TPM0_C3V = 0;				//Top left Reverse
	TPM0_C4V = 0;				//Top right Forward
	TPM0_C5V = 7500/1.2;	//Top right Reverse
	TPM1_C0V = 0;				//Bot right Forward
	TPM1_C1V = 7500/1.2;	//Bot right Reverse
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

void turnLeftAuto(){
	TPM0_C0V = 0; 			//Bot left Forward
	TPM0_C1V = 7500/1.2; 	//Bot left Reverse
	TPM0_C2V = 0;				//Top left Forward
	TPM0_C3V = 7500/1.2;	//Top left Reverse
	TPM0_C4V = 7500/1.2;	//Top right Forward
	TPM0_C5V = 0;				//Top right Reverse
	TPM1_C0V = 7500/1.2;	//Bot right Forward
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

void movement (void *argument) {
	//To implement mutext or Queue to not waste CPU cycles.
	for(;;){
		osSemaphoreAcquire(autoSelect, osWaitForever);
		osDelay(5000);
		osSemaphoreRelease(autoSelect);
	}
}


void goRoundTheObstacle(){
			  osDelay(300);
				stopBot();
				osDelay(400);
				turnLeftAuto(); //1st turn
				osDelay(450);
				forward();
				osDelay(560);
				turnRightAuto(); // 2nd turn
				osDelay(560);//     		
				forward();
				osDelay(650);
			  turnRightAuto(); // 3rd turn
				osDelay(825);	//j //STOP HERE
				forward();
				osDelay(650);
				turnRightAuto();
				osDelay(500);
				forward();
				osDelay(800);	//725
				turnLeftAuto();
				osDelay(400);
				forward();
				osDelay(900);
				stopBot();
				osDelay(400);
			  /*forward();
				osDelay(680);
			  //turnRightAuto(); // 4th turn
				//osDelay(510);//
				turnRightAuto();
				osDelay(380);
				forward();
				osDelay(615);
				
				turnLeftAuto(); // 6th turn
				//osDelay(325);
				osDelay(305);
				forward();
				osDelay(1100);
				stopBot();
				osDelay(400);*/

}



void autonomousMovement(void *argument) {
	//osSemaphoreAcquire(autoMode, osWaitForever);
	//initIRSensor();
	init_pit();
	initGreenPin();
	osDelay(400);
	forward();
	osDelay(300);
	for(;;) {
		//Move forward for a certain distance until Front_IR Triggers
		isdetect = detect();
		if (isdetect >= 1){
			PTD ->PSOR = (MASK(GREEN_LED));
			debounce = debounce + 1;
		} else{
			PTD ->PCOR = (MASK(GREEN_LED)); 
			debounce = 0;
		}
		
		if(debounce >= 4 && went_round_obstacle == 0) {
				goRoundTheObstacle();
				went_round_obstacle = 1;
		
		} else if (debounce >= 4 && went_round_obstacle == 1){
			osDelay(300);
			stopBot();
			osDelay(300);
		}
	}
}

void movechecker(){
	osDelay(400);
	forward();
	while(1){
	}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initPWM();
	initGreenPin();
	PTD ->PCOR = (MASK(GREEN_LED));
	
	int flag = 0;
	TPM0_C0V = 0;
	TPM0_C1V = 0;
	
	TPM0_C2V = 0;
	TPM0_C3V = 0;
	
	TPM0_C4V = 0;
	TPM0_C5V = 0;
	
	TPM1_C0V = 0;
	TPM1_C1V = 0;
	//init_pit();

  osKernelInitialize();                 // Initialize CMSIS-RTOS
	//autoSelect = osSemaphoreNew(1,1,NULL);
	//osThreadNew(movechecker, NULL, NULL);   
  //osThreadNew(app_main, NULL, NULL);   // Create application main thread
	//osThreadNew(movement, NULL, NULL); 		//Thread for movements
	osThreadNew(autonomousMovement,NULL,NULL);
  osKernelStart();                      // Start thread execution
	for (;;) {
	}
	
}
