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

#define TOP_LEFT_WHEEL_1	30 // PORT E pin 30 CH2
#define TOP_LEFT_WHEEL_2	29 // PORT E pin 29 CH3
#define IR_SENSOR 				13 // PORT A pin 1

#define GREEN_1  7// port C pin 7
#define GREEN_2  0// port C pin 0
#define GREEN_3  3// port C pin 3
#define GREEN_4  4// port C pin 4
#define GREEN_5  5// port C pin 5
#define GREEN_6  6// port C pin 6
#define GREEN_7  10// port C pin 10
#define GREEN_8  11// port C pin 11
#define GREEN_9  12// port C pin 12
#define GREEN_10 13// port C pin 13
#define delayGreen 0x100 // for green strip how fast u wan

#define TO_MOD(x) 375000/(x)

#define PTB0_Pin 0
#define PTB1_Pin 1

#define BUZZER 31 // port e pin 31


/*************************************************
 * Public Constants
 *************************************************/
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_AS 455
#define NOTE_DS8 4978
#define REST 0


// change this to whichever pin you want to use


// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {

  // Never Gonna Give You Up - Rick Astley
  // Score available at https://musescore.com/chlorondria_5/never-gonna-give-you-up_alto-sax
  // Arranged by Chlorondria
	 
	 
  NOTE_D5,2, NOTE_E5,8, NOTE_FS5,8, NOTE_D5,8, //13
  NOTE_E5,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, NOTE_A4,4,
  REST,2, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8,
  REST,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
	

  NOTE_E5,-8, NOTE_E5,-8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,-8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //18
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,8, NOTE_A4,8, NOTE_A4,8, 
  NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
};

 


void initPWM()
{
	//enable clock for port B
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	//enable pin output and set mux TO 3 to use alt 3 for time in port b 01 n 1
	PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);

	
	//enable clock gating for timer module
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	
	//Select clock used
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	
	
	// edge aligned pwm
	//ps: 111(128) and CMOD=01
	TPM1->SC &= ~((TPM_SC_CMOD_MASK | (TPM_SC_PS_MASK)));
	TPM1->SC |= (TPM_SC_CMOD(1) | (TPM_SC_PS(7)));
	// set to up counting
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	
	// Enable PWM on TPM1 channe; 0 -> PTB0
	//start from high output first 
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK) );
  TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

}

// change this to make the song slower or faster
int tempo = 180;
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int divider = 0, noteDuration = 0;

void setFreq(int freq)
{
	if(freq == 0)
	{
		TPM1_C0V = (unsigned int) 0;
		return;
	}
	int newMod = 375000.0 / freq;
	
	// Update TPM0's MOD
	TPM1->MOD = (unsigned int) newMod;

	// Update C0V for the buzzer
	TPM1_C0V = (unsigned int) (newMod * 0.5);
}

int isPlaying() {
	return (TPM1_C0V == (unsigned int) 0);
}



void thread_buzzer (void *argument) {
	
	
	for (;;) {
		int wholenote = (60000 * 4) / tempo;
		for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

			// calculates the duration of each note
			divider = melody[thisNote + 1];
			if (divider > 0) {
				// regular note, just proceed
				noteDuration = (wholenote) / divider;
			} else if (divider < 0) {
				// dotted notes are represented with negative durations!!
				noteDuration = (wholenote) / (divider*-1);
				noteDuration *= 1.5; // increases the duration in half for dotted notes
		}

    // we only play the note for 90% of the duration, leaving 10% as a pause
		setFreq( melody[thisNote]);
		osDelay(noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    osDelay(noteDuration);

    // stop the waveform generation before the next note.
    setFreq(0);
		osDelay(10);
	}		
 }
}


	
	

int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initPWM();
	//initGreenStrip();
	osThreadNew(thread_buzzer, NULL, NULL);
	osKernelStart();                      		// Start thread execution
	for (;;) {}
		
}


	
	
	
	
	
 
  

