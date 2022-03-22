/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
#include "MKL25Z4.h" 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#define MASK(x) (1 << (x))

#define TOP_LEFT_WHEEL_1	30 // PORT E pin 30 CH2
#define TOP_LEFT_WHEEL_2	29 // PORT E pin 29 CH3
 
 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
 
  // ...
  for (;;) {}
}

void initPWM(void)
{
	//enable clock for port B
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	//enable pin output and set mux TO 3 to use alt 3 for time in port b 01 n 1
	PORTE->PCR[TOP_LEFT_WHEEL_1] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[TOP_LEFT_WHEEL_1] |= PORT_PCR_MUX(3);
	
	PORTE->PCR[TOP_LEFT_WHEEL_2] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[TOP_LEFT_WHEEL_2] |= PORT_PCR_MUX(3);
	
	//enable clock gating for timer module
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	
	//Select clock used
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	//choose mod value
	TPM0->MOD = 7500;
	
	// edge aligned pwm
	//ps: 111(128) and CMOD=01
	TPM0->SC &= ~((TPM_SC_CMOD_MASK | (TPM_SC_PS_MASK)));
	TPM0->SC |= (TPM_SC_CMOD(1) | (TPM_SC_PS(7)));
	// set to up counting
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK);
	
	// Enable PWM on TPM1 channe; 0 -> PTB0
	//start from high output first 
	TPM0_C2SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK) );
  TPM0_C2SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	// Enable PWM on TPM1 channe; 0 -> PTB1
	TPM0_C3SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK) );
  TPM0_C3SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
}
 
/* Delay Function */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
	}
}

int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	
	initPWM();
	//TPM0_C2V = 0x0EA6;
	TPM0_C2V = 0x0EA6;
	delay(0x40000);
	
	
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
