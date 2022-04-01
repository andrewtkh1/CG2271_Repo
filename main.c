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

//Variables for interrupt
//#define BAUD_RATE 115200
#define BAUD_RATE 9600
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 64
 uint16_t rx_data = 0x00;
int counter = 0;
#define MASK(x) (1 << (x)) 

osSemaphoreId_t movementData;
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
			osSemaphoreRelease(movementData);
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
				int s = 0;
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

void blinkGreenLED(void *argument){
	while(1){
		if (isMoving == 1){
			runMode();
		} else {
			stationaryMode();
		}
	}
}
int moveCounter = 0;
void movement (void *argument) {
	//To implement mutext or Queue to not waste CPU cycles.
	uint16_t rx_data1;
	for(;;){
		//osSemaphoreAcquire(movementData, osWaitForever);
		//osMessageQueueGet(movementQ, &rx_data1, NULL, osWaitForever);
		//rx_data = rx_data1;
		moveCounter++;
		//if (rx_data == 0x0) {
			//stopBot();
			//isMoving = 0;
		if (rx_data == 0x1) {
			forward();
			isMoving = 1;
		} else if (rx_data == 0x2) {
			reverse();
			isMoving = 1;
		} else if (rx_data == 0x3) {
			turnLeft();
			isMoving = 1;
		} else if (rx_data == 0x4) {
			turnRight();
			isMoving = 1;
		} else if (rx_data == 0x5) {
			forwardRight();
			isMoving = 1;
		} else if (rx_data == 0x6) {
			forwardLeft();
			isMoving = 1;
		} else {
			stopBot();
			isMoving = 0;
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
  // ...
	
	
	
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	//osThreadNew(movement,NULL, &moveAtr); 		//Thread for movements
	//movementQ = osMessageQueueNew(3,sizeof(rx_data), NULL);
	osThreadNew(movement, NULL, NULL);
	osThreadNew(blinkRedLED, NULL, NULL);
	//osThreadNew(blinkGreenLED, NULL, NULL);
	movementData = osSemaphoreNew(1,0,NULL);
  osKernelStart();                      // Start thread execution
  //int flag = 0;
	for (;;) {
	}
}
