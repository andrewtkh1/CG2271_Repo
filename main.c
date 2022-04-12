/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h"
#include "system_MKL25Z4.h"   	         // Keil::Device:Startup
#include "cmsis_os2.h"
#include "movements.h"
#include "LEDControl.h"
#include "buzzer.h"
//#include "autoMoves.h"
//#include "autoMoves.c"

//Variables for interrupt
//#define BAUD_RATE 115200
#define BAUD_RATE 9600
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 64
#define MASK(x) (1 << (x))



volatile uint16_t rx_data = 0x00;
uint16_t moveData = 0x0;
int counter = 0;
int debounce = 0;

osSemaphoreId_t recieveData, manualMode, autoMode;
osMessageQueueId_t movementQ;

void initUART2Interrupt(){
  //Enable RIE so that when we are ready to receive, the interrupt would also be triggered and we can do some action.
	// set piroity fot interrupt uart
	NVIC_SetPriority(UART2_IRQn, UART2_INT_PRIO);
	NVIC_ClearPendingIRQ(UART2_IRQn);
	UART2->C2 |= UART_C2_RIE_MASK;
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

	initUART2Interrupt();
	//No pairity, 8 bits
	UART2->C1 = 0;
	UART2->S2 = 0;
	UART2->C3 = 0;
	
	// enable RX
	UART2->C2 |= (UART_C2_RE_MASK);
}

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

//UART2 ISR
void UART2_IRQHandler(void){
	   if (UART2->S1 & UART_S1_RDRF_MASK) {
			rx_data = UART2->D;
			counter++;
			//osMessageQueuePut(movementQ, &rx_data, NULL, 0);
			osSemaphoreRelease(recieveData);
		 }
		 
		 if (UART2->S1 & UART_S1_TDRE_MASK) {
			 //JUst to clear flags.
		 }
		 		// handle the error
				// clear the flag
		 if (UART2->S1 & (UART_S1_OR_MASK |
			UART_S1_NF_MASK | 
			UART_S1_FE_MASK | 
			UART_S1_PF_MASK)) {
			}
}

int moveCounter = 0;
void movement (void *argument) {
	//To implement mutext or Queue to not waste CPU cycles.
	//uint16_t rx_data;
	osSemaphoreAcquire(manualMode,osWaitForever); //Means it is maunal mode.
	for(;;){
		//osMessageQueueGet(movementQ, &rx_data1, NULL, osWaitForever);
		//osDelay(30);
		//rx_data = rx_data1;
		//moveCounter++;
		switch (moveData){
			case 0x0:
				stopBot();
				isMoving = 0;
				break;
			case 0x1:
				forwardManual();
				isMoving = 1;
				break;
			case 0x2:
				reverse();
				isMoving = 1;
				break;
			case 0x3:
				turnLeft();
				isMoving = 1;
				break;
			case 0x4:
				turnRight();
				isMoving = 1;
				break;
			case 0x5:
				forwardRight();
				//forwardRightAuto();
				isMoving = 1;
				break;
			case 0x6:
				forwardLeft();
				isMoving = 1;
				break;
		}
	}
}

void decode (void *argument) {
	while(1){
		osSemaphoreAcquire(recieveData, osWaitForever);
		if (rx_data <= 0x6){
			moveData = rx_data;
			osSemaphoreRelease(manualMode);
		}
		if (rx_data == 0x07){
			//RUN AUTO FUNCTION.
			osSemaphoreRelease(autoMode);
		}
		if (rx_data == 0x08){
			isFinished = 0;
		}
		if (rx_data == 0x09){
			isFinished = 1;
		}
	}
}

void autonomousMovement(void *argument) {
	osSemaphoreAcquire(autoMode, osWaitForever);
	//initIRSensor();
	init_pit();
	initGreenPin();
	osDelay(400);
	forward();
	osDelay(300);
	isMoving = 1;
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
			//goUpRight();
			went_round_obstacle = 1;
			isFinished = 1;
			isMoving = 0;
			debounce = 0;
			} else if (debounce >= 4 && went_round_obstacle == 1){
			osDelay(300);
			stopBot();
			osDelay(300);
			}
		}
}

 
const osThreadAttr_t moveAtr = {
  .priority = osPriorityHigh                    //Set initial thread priority to high   
};
 

int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initPWM();
	initUART2(BAUD_RATE);
	initRedLED();
	initGreenStrip();
	initBuzzer();
	
  // ...
	
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	//osThreadNew(movement,NULL, &moveAtr); 		//Thread for movements priorty
	//movementQ = osMessageQueueNew(3,sizeof(rx_data), NULL);
	
	//threads
	osThreadNew(movement, NULL, NULL);
	osThreadNew(blinkRedLED, NULL, NULL);
	osThreadNew(blinkGreenLED, NULL, NULL);
	osThreadNew(buzzer, NULL, NULL);
	osThreadNew(decode,NULL, NULL);
	osThreadNew(autonomousMovement, NULL, NULL);
	
	//semaphores
	recieveData = osSemaphoreNew(1,0,NULL);
	manualMode = osSemaphoreNew(1,0,NULL);
	autoMode = osSemaphoreNew(1,0,NULL);
	
  osKernelStart();                      // Start thread execution
  //int flag = 0;
	for (;;) {
	}
}
