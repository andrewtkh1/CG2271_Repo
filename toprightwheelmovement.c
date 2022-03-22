/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h" // Device header
#define PTA5_Pin 5 //CHANNEL 2
#define PTA4_Pin 4 //CHANNEL 1
#define MASK(x)	(1 << (x))
 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
 
  // ...
  for (;;) {}
}

void InitPWM()
{
	//Enable Clock to PORTB.
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	//Configure MUX settings to make all 3 pins GPIO
	
	//Zero out the entire register for PTB4 Pin
	PORTA->PCR[PTA4_Pin] &= ~PORT_PCR_MUX_MASK;
	//Set the MUX portion of the PTB4 Pin register to be ALT 3 mode.
	PORTA->PCR[PTA4_Pin] |= PORT_PCR_MUX(3);
	
	//Zero out the entire register for PTB0 Pin
	PORTA->PCR[PTA5_Pin] &= ~PORT_PCR_MUX_MASK;
	//Set the MUX portion of the PTB1 Pin register to be ALT 3 mode.
	PORTA->PCR[PTA5_Pin] |= PORT_PCR_MUX(3);
	
	//Set the clock gating for TPM1 and TPM0
	SIM->SCGC6 |=  SIM_SCGC6_TPM0_MASK ;
	
	//Select the appropriate Internal Clock we use the TPMSRC clock.
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	//Set to use internal clock for TPM0
	TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM0->SC |= ((TPM_SC_CMOD(1)) | (TPM_SC_PS(7)));  //Divide by 128 prescalar
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK); //Set to upcounting mode
	
	//Set the counter to run as low true Pass for TPM0.
	TPM0_C1SC &= ~((TPM_CnSC_ELSB_MASK | (TPM_CnSC_ELSA_MASK) |(TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK)));
	TPM0_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	TPM0_C2SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK) );
	TPM0_C2SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	TPM0->MOD = 7499;
}

void topRightWheelForward(){
		TPM0_C1V = 3750;
		TPM0_C2V = 0;
}

void topRightWheelBackward(){
		TPM0_C1V = 0;
		TPM0_C2V = 3750;
}
 
static void delay(volatile uint32_t nof){
	while(nof!=0){
		__asm("NOP");
		nof--;
	}
}

int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
  // ...
	InitPWM();
  //osKernelInitialize();                 // Initialize CMSIS-RTOS
  //osThreadNew(app_main, NULL, NULL);    // Create application main thread
  //osKernelStart();   	// Start thread execution
	while(1){
	delay(0x800000);
	topRightWheelForward();
	delay(0x800000);
	topRightWheelBackward();
	}
  for (;;) {}
}
