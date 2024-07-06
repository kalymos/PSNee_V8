// PSNee V8 - Ultimate PSX unlocker.   /ver. 8.0.1
// Developed by brill & postal2201, based on PSNee V7 open source project.   /Emu-land.net


//Configuring the clock speed and associated registers.  F_CPU / (TCCR0B |= (1<<CS00) *(OCR0A = 159 +1) = 16000000 /(0 * (160)) = 100KHz

//------------------------------------------------------------------------------------------------
//                         Select your chip
//------------------------------------------------------------------------------------------------

#define ATmega328_168      // Fuses: JAP_FAT - H: DF, L: EE, E: FF; Other - H: DF, L: FF, E: FF.
/*  PinVCC-3.5v, PinGND-GND, Pin2-BIOS AX, Pin3-BIOS AY, Pin4-BIOS DX, Pin5-Switch*(optional),
    Pin6-SQCK, Pin7-SUBQ, Pin8-DATA, Pin9-GATE_WFCK, PinRST-RESET*(Only for JAP_FAT)  */

//------------------------------------------------------------------------------------------------
//                         Select your console
//------------------------------------------------------------------------------------------------
//                              Attention!
//   If a BIOS checksum is specified, it is more important than the SCPH model number!
//------------------------------------------------------------------------------------------------	

//#define UC_ALL           // Use for all NTSC-U/C models. No BIOS patching needed.
#define PAL_FAT          // Use for all PAL FAT models. No BIOS patching needed.
//#define SCPH_103         // No BIOS patching needed.
//#define SCPH_102         // DX - D0, AX - A7. BIOS ver. 4.4e, CRC 0BAD7EA9 | 4.5e, CRC 76B880E5
//#define SCPH_100         // DX - D0, AX - A7. BIOS ver. 4.3j, CRC F2AF798B
//#define SCPH_7000_9000   // DX - D0, AX - A7. BIOS ver. 4.0j, CRC EC541CD0
//#define SCPH_5500        // DX - D0, AX - A5. BIOS ver. 3.0j, CRC FF3EEB8C
//#define SCPH_3500_5000   // DX - D0, for 40-pin BIOS: AX - A4, for 32-pin BIOS: AX - A5. BIOS ver. 2.2j, CRC 24FC7E17 | 2.1j, CRC BC190209
//#define SCPH_3000        // DX - D5, for 40-pin BIOS: AX - A6, AY - A7, for 32-pin BIOS: AX - A7, AY - A8. BIOS ver. 1.1j, CRC 3539DEF6
//#define SCPH_1000        // DX - D5, for 40-pin BIOS: AX - A6, AY - A7, for 32-pin BIOS: AX - A7, AY - A8. BIOS ver. 1.0j, CRC 3B601FC8

//------------------------------------------------------------------------------------------------
//                         Options
//------------------------------------------------------------------------------------------------

//#define AUTOREGION         // If ensable, send all SCEX codes, instead of the code selected model.
#define PATCH_SW           // Enables hardware support for disabling BIOS patching.
#define LED_RUN

//------------------------------------------------------------------------------------------------
//                         Code section
//------------------------------------------------------------------------------------------------

#include "config.h"
#include "settings.h"
#include "patching.h"
#include "finction.h"


//Creation of the different variables for the counter
volatile uint8_t count_isr = 0;
volatile uint32_t microsec = 0;
volatile uint16_t millisec = 0;

//Initializing the flag for the pin wfck
bool wfck_mode = 0;

// borrowed from AttyNee. Bitmagic to get to the SCEX strings stored in flash (because Harvard architecture)
// Read a specific bit from an array of bytes
uint8_t readBit(uint8_t index, const uint8_t * ByteSet)
{
	return !!(ByteSet[index / 8] & (1 << (index % 8)));       // Return true if the specified bit is set in ByteSet[index]
}



// Static arrays storing SCEX data for different regions
void inject_SCEX(const char region)
{
	static const uint8_t SCEEData[] = {
		0b01011001,
		0b11001001,
		0b01001011,
		0b01011101,
		0b11101010,
		0b00000010
	};
	
	static const uint8_t SCEAData[] = {
		0b01011001,
		0b11001001,
		0b01001011,
		0b01011101,
		0b11111010,
		0b00000010
	};
	
	static const uint8_t SCEIData[] = {
		0b01011001,
		0b11001001,
		0b01001011,
		0b01011101,
		0b11011010,
		0b00000010
	};

	// pinMode(data, OUTPUT) is used more than it has to be but that's fine. 
	
	// Iterate through 44 bits of SCEX data
	for (uint8_t bit_counter = 0; bit_counter < 44; bit_counter++)
	{
		// Check if the current bit is 0
		if (readBit(bit_counter, region == 'e' ? SCEEData : region == 'a' ? SCEAData : SCEIData) == 0)
		{
			PIN_DATA_OUTPUT;                                         // Set DATA output
			PIN_DATA_CLEAR;                                          // Set DATA pin low
			_delay_us(delay_between_bits);                       // Wait for specified delay between bits
		}

		else
		{
			// modulate DATA pin based on WFCK_READ
			if (wfck_mode)                                      // If wfck_mode is true(pu22mode)
			{
				PIN_DATA_OUTPUT;                                    // Set DATA output
				Timer_Start();                                  // Start timer for precise timing
			 do
			 {
				 // read wfck pin
				if(PIN_WFCK_READ)
				{
					PIN_DATA_SET;                                   // Set DATA pin high
				}

				else
				{
					PIN_DATA_CLEAR;                                 // Set DATA pin low
				}
			 }

			while (microsec < delay_between_bits);
			Timer_Stop();                                       // Stop timer
			}
            
            // PU-18 or lower mode
			else
			{
				PIN_DATA_INPUT;
				_delay_us(delay_between_bits);
			}
		}
	}

	// After injecting SCEX data, set DATA pin as output and clear (low)
	PIN_DATA_OUTPUT;
	PIN_DATA_CLEAR;
	_delay_ms(delay_between_injections);
}

int main()
{
	uint8_t hysteresis = 0;
	uint8_t scbuf[12] = {0};                                // SUBQ bit storage
	uint16_t timeout_clock_counter = 0;
	uint8_t bitbuf = 0;
	uint8_t bitpos = 0;
	uint8_t scpos = 0;                                     // scbuf position
	uint16_t highs = 0, lows = 0;                          //highs = 0,



 
	#ifdef AUTOREGION
	 const char region[3] = {'e', 'a', 'i'};
	#endif

	Init();

	#ifdef LED_RUN
	 LED_ON;
	#endif

	#if defined(BIOS_PATCH) && !defined(PATCH_SW)
	 Bios_Patching();
	#elif defined(BIOS_PATCH) && defined(PATCH_SW)
	 if (SW_CHECK != 0)
	 {
	 	Bios_Patching();
	 }
	 else
	 {
	 while (PIN_SQCK_READ == 0);
	 while (PIN_WFCK_READ == 0); 
	 }
	 // wait for console power on and stable signals
	#else
	 while (PIN_SQCK_READ == 0);
	 while (PIN_WFCK_READ == 0);
	#endif

	Timer_Start();
	
	  // Board detection
  //
  // GATE: __-----------------------  // this is a PU-7 .. PU-20 board!
  //
  // WFCK: __-_-_-_-_-_-_-_-_-_-_-_-  // this is a PU-22 or newer board!
	
	do
	{
		if (PIN_WFCK_READ == 1) highs++;          // pas util?
		if (PIN_WFCK_READ == 0) lows++;           // good for ~5000 reads in 1s
		_delay_us(200);
	}
	while (millisec < 1000);                  // sample 1s

	Timer_Stop();
	
	      // typical readouts
  // PU-22: highs: 2449 lows: 2377

	if (lows > 100)
	{
		wfck_mode = 1;                       //flag pu22mode
	}

	else
	{
		wfck_mode = 0;                       //flag oldmod
	}

	#ifdef LED_RUN
	 LED_OFF;
	#endif

	while(1)
	{
		_delay_ms(1);                                           /* Start with a small delay, which can be necessary 
                                                                |  in cases where the MCU loops too quickly and picks up the laster SUBQ trailing end
																*/ 
		// start critical section
		GLOBAL_INTERRUPT_DISABLE;                                                     
		
		// Capture 8 bits for 12 runs > complete SUBQ transmission
		do
		{
			for (bitpos = 0; bitpos < 8; bitpos++)
			{
				while (PIN_SQCK_READ != 0)                           // wait for clock to go low
				{
					timeout_clock_counter++;                     // a timeout resets the 12 byte stream in case the PSX sends malformatted clock pulses, as happens on bootup
					if (timeout_clock_counter > 1000)            
					{
						scpos = 0;                               // Reset SUBQ packet stream
						timeout_clock_counter = 0;               // Reset
						bitbuf = 0;                              // Reset
						bitpos = 0;                              // Reset
						continue;
					}
				}
				
				while (PIN_SQCK_READ == 0);                          // Wait for clock to go high
				
				if(PIN_SUBQ_READ)                                    // If clock pin high
				{
					bitbuf |= 1 << bitpos;                       // Set the bit at position bitpos in the bitbuf to 1. Using OR combined with a bit shift
				}
				
				timeout_clock_counter = 0;                       // no problem with this bit
			}

			scbuf[scpos] = bitbuf;                               // One byte done
			scpos++;
			bitbuf = 0;
		}

		while (scpos < 12);                                     // Repeat for all 12 bytes
		// End critical section
		GLOBAL_INTERRUPT_ENABLE;                                                  // End critical section
		
		/* Check if read head is in wobble area
      |  We only want to unlock game discs (0x41) and only if the read head is in the outer TOC area.
      |  We want to see a TOC sector repeatedly before injecting (helps with timing and marginal lasers).
      |  All this logic is because we don't know if the HC-05 is actually processing a getSCEX() command.
      |  Hysteresis is used because older drives exhibit more variation in read head positioning.
      |  While the laser lens moves to correct for the error, they can pick up a few TOC sectors.
      */

		uint8_t isDataSector = (((scbuf[0] & 0x40) == 0x40) && (((scbuf[0] & 0x10) == 0) && ((scbuf[0] & 0x80) == 0)));

		if (
		(isDataSector && scbuf[1] == 0x00 && scbuf[6] == 0x00) &&                 // [0] = 41 means psx game disk. the other 2 checks are garbage protection
		(scbuf[2] == 0xA0 || scbuf[2] == 0xA1 || scbuf[2] == 0xA2 ||              // if [2] = A0, A1, A2 ..
		(scbuf[2] == 0x01 && (scbuf[3] >= 0x98 || scbuf[3] <= 0x02)))             // .. or = 01 but then [3] is either > 98 or < 02
		)
		{
			hysteresis++;
		}

		else if (hysteresis > 0 && ((scbuf[0] == 0x01 || isDataSector) && (scbuf[1] == 0x00 /*|| scbuf[1] == 0x01*/ ) && scbuf[6] == 0x00))
		{
			hysteresis++;                                  // This CD has the wobble into CD-DA space. (started at 0x41, then went into 0x01)
		}

		else if (hysteresis > 0)
		{
			hysteresis--;                                 // None of the above. Initial detection was noise. Decrease the counter.
		}
		// hysteresis value "optimized" using very worn but working drive on ATmega328 @ 16Mhz
		// should be fine on other MCUs and speeds, as the PSX dictates SUBQ rate
		if (hysteresis >= 14)
		{
			// If the read head is still here after injection, resending should be quick.
			// Hysteresis naturally goes to 0 otherwise (the read head moved).
			hysteresis = 11;
			
			#ifdef LED_RUN
			 LED_ON;
			#endif

			PIN_DATA_OUTPUT;                               // Set DATA pin output
			PIN_DATA_CLEAR;                                // Set DATA pin pul down

			if (!wfck_mode)                           // If wfck_mode is fals (oldmode)
			{
				PIN_WFCK_OUTPUT;                          // Set WFCK pin output
				PIN_WFCK_CLEAR;                           // SET WFCK pin pul down
			}
			
			_delay_ms(delay_between_injections);      // HC-05 waits for a bit of silence (pin low) before it begins decoding.

	    // inject symbols now. 2 x 3 runs seems optimal to cover all boards
			for (uint8_t scex = 0; scex < 2; scex++)          
			{
				inject_SCEX(region[scex]);            // If wfck_mode is fals (oldmode)
			}

			if (!wfck_mode)                           // Set WFCK pin input
			{
				PIN_WFCK_INPUT;                           // Set DATA pin input
			}

			PIN_DATA_INPUT;

			#ifdef LED_RUN
			 LED_OFF;
			#endif
		}
	}
}
