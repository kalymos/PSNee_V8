


//void Timer_Start(void);
//void Timer_Stop(void);
////#include <avr/interrupt.h>
////#include <avr/sfr_defs.h>
////#include <util/delay.h>
//
//extern volatile uint32_t microsec;
//extern volatile uint16_t millisec;
//extern volatile boolean wfck_mode;
//
//extern const uint16_t DELAY_BETWEEN_BITS;
//extern const uint8_t DELAY_BETWEEN_INJECTIONS;
//extern const char region[3];
//
////#ifdef SCEI
////const char region[3] = {'i', 'i', 'i'};
////#endif
//
//// borrowed from AttyNee. Bitmagic to get to the SCEX strings stored in flash (because Harvard architecture)
//// Read a specific bit from an array of bytes
//uint8_t readBit(uint8_t index, const uint8_t * ByteSet)
//{
//  return !!(ByteSet[index / 8] & (1 << (index % 8)));       // Return true if the specified bit is set in ByteSet[index]
//}
//
//
//// Static arrays storing SCEX data for different regions
//void inject_SCEX(const char region)
//{
//  static const uint8_t SCEEData[] = {
//    0b01011001,
//    0b11001001,
//    0b01001011,
//    0b01011101,
//    0b11101010,
//    0b00000010
//  };
//  
//  static const uint8_t SCEAData[] = {
//    0b01011001,
//    0b11001001,
//    0b01001011,
//    0b01011101,
//    0b11111010,
//    0b00000010
//  };
//  
//  static const uint8_t SCEIData[] = {
//    0b01011001,
//    0b11001001,
//    0b01001011,
//    0b01011101,
//    0b11011010,
//    0b00000010
//  };
//
//  // pinMode(data, OUTPUT) is used more than it has to be but that's fine. 
//  
//  // Iterate through 44 bits of SCEX data
//  for (uint8_t bit_counter = 0; bit_counter < 44; bit_counter++)
//  {
//    // Check if the current bit is 0
//    if (readBit(bit_counter, region == 'e' ? SCEEData : region == 'a' ? SCEAData : SCEIData) == 0)
//    {
//      PIN_DATA_OUTPUT;                                         // Set DATA output
//      PIN_DATA_CLEAR;                                          // Set DATA pin low
//      _delay_us(DELAY_BETWEEN_BITS);                       // Wait for specified delay between bits
//    }
//
//    else
//    {
//      // modulate DATA pin based on WFCK_READ
//      if (wfck_mode)                                      // If wfck_mode is true(pu22mode)
//      {
//        PIN_DATA_OUTPUT;                                    // Set DATA output
//        Timer_Start();                                  // Start timer for precise timing
//       do
//       {
//         // read wfck pin
//        if(PIN_WFCK_READ)
//        {
//          PIN_DATA_SET;                                   // Set DATA pin high
//        }
//
//        else
//        {
//          PIN_DATA_CLEAR;                                 // Set DATA pin low
//        }
//       }
//
//      while (microsec < DELAY_BETWEEN_BITS);
//      Timer_Stop();                                       // Stop timer
//      }
//            
//            // PU-18 or lower mode
//      else
//      {
//        PIN_DATA_INPUT;
//        _delay_us(DELAY_BETWEEN_BITS);
//      }
//    }
//  }
//
//  // After injecting SCEX data, set DATA pin as output and clear (low)
//  PIN_DATA_OUTPUT;
//  PIN_DATA_CLEAR;
//  _delay_ms(DELAY_BETWEEN_INJECTIONS);
//}
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
		 DX_SET;                         // DX pull up
		#endif
		DX_OUTPUT;                       // DX is configured as output.
		PATCHING;                        // PATCHING _delay_us
		#ifdef HIGH_PATCH
		 DX_CLEAR;                       // DX pull down
		#endif
		DX_INPUT;                        // DX is reconfigured as input
		AX_INTERRUPT_DISABLE;            // The interrupt for AX is disabled (AX_INTERRUPT_DISABLE)..

		impulse = 0;                     // impulse is reset to 0.
		patch = 1;                       // patch is set to 1, indicating that the first patch is completed.
	}
}

#ifdef DOUBLE_PATCH                  // If DOUBLE_PATCH is defined

// ISR for AY (INT1_vect):
ISR(INT1_vect){

	impulse++;                         // The variable impulse is incremented.
	if (impulse == TRIGGER2)           // If impulse reaches the value defined by TRIGGER2, the following actions are performed:
	{
		HOLD2;                           // HOLD2 _delay_us
		DX_OUTPUT;                       // DX is configured as output.
		PATCHING2;                       // PATCHING2 _delay_us
		DX_INPUT;                        // DX is reconfigured as input.
		AY_INTERRUPT_DISABLE;            // The interrupt for AY is disabled (AY_INTERRUPT_DISABLE).

		patch = 2;                       // patch is set to 2, indicating that the second patch is completed.
	}
}

#endif

void Bios_Patching(){

	#ifdef LOW_TRIGGER                // If LOW_TRIGGER is defined
	 AX_INTERRUPT_FALLING;            // The interrupt is triggered on a falling edge,
	 #else
	 AX_INTERRUPT_RISING;             // Otherwise on a rising edge.
	#endif

	if (AX_READ != 0)                 // If the AX pin is high
	{
		while (AX_READ != 0);           // Wait for it to go low
		while (AX_READ == 0);           // Then wait for it to go high again.
	}
	else                              // If the AX pin is low
	{
		while (AX_READ == 0);           // Wait for it to go high.
	}
	
	Timer_Start();                    // Start a timer:
	while (microsec < CHECKPOINT);    // Wait until the number of microseconds elapsed reaches a value defined by CHECKPOINT.
	Timer_Stop();                     // Stop the timer.
	AX_INTERRUPT_ENABLE;              // Enable the interrupt for the AX pin.
	
	while (patch != 1);               // Wait for the first stage of the patch to complete:

	#ifdef DOUBLE_PATCH

	 #ifdef LOW_TRIGGER2             // If LOW_TRIGGER2 is defined.
	  AY_INTERRUPT_FALLING;          // The interrupt is triggered on a falling edge.
	 #else
	  AY_INTERRUPT_RISING;           // Otherwise on a rising edge.
	 #endif

	while (AY_READ != 0);            // Wait for a specific state of the AY pin:

	Timer_Start();                   // Start a timer:

	while (microsec < CHECKPOINT2);  // Wait until the number of microseconds elapsed reaches a value defined by CHECKPOINT2.
	Timer_Stop();                    // Stop the timer.
	AY_INTERRUPT_ENABLE;             // Enable the interrupt for the AY pin.

	while (patch != 2);              // Wait for the second stage of the patch to complete:

	#endif

}

#endif
