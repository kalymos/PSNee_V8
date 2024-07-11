
#pragma once

#ifdef BIOS_PATCH

void Timer_Start(void);
void Timer_Stop(void);

extern volatile uint8_t count_isr;
extern volatile uint32_t microsec;
extern volatile uint16_t millisec;

volatile uint8_t impulse = 0;
volatile uint8_t patch = 0;

// ISR for AX (INT0_vect):
ISR(INT0_vect) {
	impulse++;                         // The variable impulse is incremented.
	if (impulse == TRIGGER){           // If impulse reaches the value defined by TRIGGER, the following actions are performed:
		HOLD;                            // HOLD _delay_us
		#ifdef HIGH_PATCH                // If HIGH_PATCH is defined
		 PIN_DX_SET;                         // DX pull up
		#endif
		PIN_DX_OUTPUT;                       // DX is configured as output.
		PATCHING;                        // PATCHING _delay_us
		#ifdef HIGH_PATCH
		 PIN_DX_CLEAR;                       // DX pull down
		#endif
		PIN_DX_INPUT;                        // DX is reconfigured as input
		PIN_AX_INTERRUPT_DISABLE;            // The interrupt for AX is disabled (AX_INTERRUPT_DISABLE)..

		impulse = 0;                     // impulse is reset to 0.
		patch = 1;                       // patch is set to 1, indicating that the first patch is completed.
	}
}

#ifdef HIGH_PATCH                   // If DOUBLE_PATCH is defined

// ISR for AY (INT1_vect):
ISR(INT1_vect){

	impulse++;                         // The variable impulse is incremented.
	if (impulse == TRIGGER2)           // If impulse reaches the value defined by TRIGGER2, the following actions are performed:
	{
		HOLD2;                           // HOLD2 _delay_us
		PIN_DX_OUTPUT;                       // DX is configured as output.
		PATCHING2;                       // PATCHING2 _delay_us
		PIN_DX_INPUT;                        // DX is reconfigured as input.
		PIN_AY_INTERRUPT_DISABLE;            // The interrupt for AY is disabled (AY_INTERRUPT_DISABLE).

		patch = 2;                       // patch is set to 2, indicating that the second patch is completed.
	}
}

#endif

void Bios_Patching(){

	#ifdef LOW_TRIGGER                // If LOW_TRIGGER is defined
	 PIN_AX_INTERRUPT_FALLING;            // The interrupt is triggered on a falling edge,
	 #else
	 PIN_AX_INTERRUPT_RISING;             // Otherwise on a rising edge.
	#endif

	if (PIN_AX_READ != 0)                 // If the AX pin is high
	{
		while (PIN_AX_READ != 0);           // Wait for it to go low
		while (PIN_AX_READ == 0);           // Then wait for it to go high again.
	}
	else                              // If the AX pin is low
	{
		while (PIN_AX_READ == 0);           // Wait for it to go high.
	}
	
	Timer_Start();                    // Start a timer:
	while (microsec < CHECKPOINT);    // Wait until the number of microseconds elapsed reaches a value defined by CHECKPOINT.
	Timer_Stop();                     // Stop the timer.
	PIN_AX_INTERRUPT_ENABLE;              // Enable the interrupt for the AX pin.
	
	while (patch != 1);               // Wait for the first stage of the patch to complete:

	#ifdef HIGH_PATCH 

	 #ifdef HIGH_PATCH              // If LOW_TRIGGER2 is defined.
	  PIN_AY_INTERRUPT_FALLING;          // The interrupt is triggered on a falling edge.
	 #else
	  PIN_AY_INTERRUPT_RISING;           // Otherwise on a rising edge.
	 #endif

	while (PIN_AY_READ != 0);            // Wait for a specific state of the AY pin:

	Timer_Start();                   // Start a timer:

	while (microsec < CHECKPOINT2);  // Wait until the number of microseconds elapsed reaches a value defined by CHECKPOINT2.
	Timer_Stop();                    // Stop the timer.
	PIN_AY_INTERRUPT_ENABLE;             // Enable the interrupt for the AY pin.

	while (patch != 2);              // Wait for the second stage of the patch to complete:

	#endif

}

#endif
