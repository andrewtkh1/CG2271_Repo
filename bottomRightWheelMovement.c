/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
#include "MKL25Z4.h"                    // Device header
#include "system_MKL25Z4.h"             // Keil::Device:Startup
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#define PTE21_Pin 21
#define PTE20_Pin 20
 
enum wheel_Control {
	BR_Forward = PTE21_Pin, BR_Backward = PTE20_Pin};
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void initPWM(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;	//ON clock for Port E
	
	PORTE->PCR[PTE21_Pin] &= ~PORT_PCR_MUX_MASK;	//Clearing the bits of Port E to 0
	PORTE->PCR[PTE21_Pin] |= PORT_PCR_MUX(3);		//Setting it to be PWM. 3 = PWM.
	
	PORTE->PCR[PTE20_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE20_Pin] |= PORT_PCR_MUX(3);
	
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;	//ENABLE TIMER FOR PWM TO WORK
	
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;		//CLEARING THE BITS ON SOPT2 REGISTER, BITS FOR TPMSRC
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);	//SETTING TPMSRC VALUE TO 1
	
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK)); //CMOD = clock mode selection, PS = Prescale Factor Selection
	TPM1->SC |= ((TPM_SC_CMOD(1)) | (TPM_SC_PS(7)));	//PS = 128
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);	//Set PWM to Up counting mode
	
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));	//Setting whether its count up/down. MSB = Select edge aligned.
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));	//Setting whether its count up/down. MSB = Select edge aligned.
	
	TPM1->MOD = 7499;	//VALUE OF 1 PWM CYCLE
	
}

static void delay(volatile uint32_t nof) {
	while(nof!=0) {
		__asm("NOP");
		nof--;
	}
}

void bottomRightForward () {
 //void *argument
  // ...
  TPM1_C0V = 0x0EA6;	//Cmp value
	TPM1_C1V = 0x0;	
}

void bottomRightBackward () {
 //void *argument
  // ...
  TPM1_C0V = 0x00;	//Cmp value
	TPM1_C1V = 0x0EA6;	
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initPWM();
  // ...
	while(1){
		bottomRightForward();
		delay(0x800000);
		bottomRightBackward();
		delay(0x800000);
	}
  //osKernelInitialize();                 // Initialize CMSIS-RTOS
  //osThreadNew(bottomRight, NULL, NULL);    // Create application main thread
  //osKernelStart();                      // Start thread execution
  //for (;;) {}
}
