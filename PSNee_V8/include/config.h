#pragma once



#ifdef ATmega328_168

#define F_CPU 16000000L
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

// Handling the main pins
// Main pins input
//Create a mask (1<<0) with the first bit at 1 b00000001 uses the ~ operator to perform a bit inversion b11111110 , &= updates the DDRB register with the AND operator and the mask, DDRB bxxxxxxxx OR mask b11111110 = bxxxxxxx0
#define PIN_DATA_INPUT      DDRB  &= ~(1<<DDB0)                             //
#define PIN_WFCK_INPUT      DDRB  &= ~(1<<DDB1)                             //
#define PIN_SQCK_INPUT      DDRD  &= ~(1<<DDD6)                             //  PSX subchannel clock
#define PIN_SUBQ_INPUT      DDRD  &= ~(1<<DDD7)                             //  PSX subchannel bits
//Main pin output
//Create a mask (1<<0) with the first bit at 1 b00000001, |= updates the DDRB register with the OR operator and the mask, DDRB bxxxxxxxx OR mask b00000001 = bxxxxxxx1
#define PIN_DATA_OUTPUT     DDRB  |=  (1<<DDB0)                             //
#define PIN_WFCK_OUTPUT     DDRB  |=  (1<<DDB1)                             //
//Define pull-ups and set high at the main pin
//Create a mask (1<<0) with the first bit at 1 b00000001, |= updates the PORTB register with the OR operator and the mask, PORTB bxxxxxxxx OR mask b00000001 = bxxxxxxx1
#define PIN_DATA_SET        PORTB |=  (1<<PB0)                              //
//Define pull-ups set down at the main pin
//Create a mask (1<<0) with the first bit at 1 b00000001 uses the ~ operator to perform a bit inversion b11111110 , &= updates the DDRB register with the AND operator and the mask, DDRB bxxxxxxxx OR mask b11111110 = bxxxxxxx0
#define PIN_DATA_CLEAR      PORTB &= ~(1<<PB0)                              //
#define PIN_WFCK_CLEAR      PORTB &= ~(1<<PB1)                              //
//Read the main pins
//Create a mask (1<<6) with the six bit at 1 b00100000, compare the PINB register and the mask with the AND operator, and returns the result, PINB bxx1xxxxx AND mask b00100000 = 1, 
#define PIN_SQCK_READ       (PIND & (1<<PIND6))                             //
#define PIN_SUBQ_READ       (PIND & (1<<PIND7))                             //
#define PIN_WFCK_READ       (PINB & (1<<PINB1))                             //

#define TIMER_INTERRUPT_ENABLE      TIMSK0 |= (1<<OCIE0A)
#define TIMER_INTERRUPT_DISABLE     TIMSK0 &= ~(1<<OCIE0A)
#define TIMER_TCNT_CLEAR            TCNT0 = 0x00
#define TIMER_TIFR_CLEAR            TIFR0 |= (1<<OCF0A)

#define AX_INTERRUPT_ENABLE         EIMSK = (1<<INT0)
#define AX_INTERRUPT_DISABLE        EIMSK &= ~(1<<INT0)
#define AX_INTERRUPT_RISING         EICRA = (1<<ISC01)|(1<<ISC00)
#define AX_INTERRUPT_FALLING        EICRA = (1<<ISC01)

#define AY_INTERRUPT_ENABLE         EIMSK = (1<<INT1)
#define AY_INTERRUPT_DISABLE        EIMSK &= ~(1<<INT1)
#define AY_INTERRUPT_RISING         EICRA = (1<<ISC11)|(1<<ISC10)
#define AY_INTERRUPT_FALLING        EICRA = (1<<ISC11)

// Handling and reading the switch pin for patch BIOS
//BIOS switch pin input
#define SW_INPUT          DDRD  &= ~(1<<DDD5)                              //INPUT     D5
//Define pull-ups and set high at BIOS switch pin
#define SW_USE            PORTD |= (1<<PD5)                                //Pull_hup  D5
//Read pin BIOS switch
#define SW_CHECK          (PIND &  (1<<PIND5))                             //Read      D5

// Handling and use of the LED pin
#define LED_RUN
//LED pin output
#define LED_OUTPUT        DDRB  |=  (1<<DDB5)                                //OUTPUT    D13
//led pin on
#define LED_ON            PORTB |=  (1<<PB5)                                 //Pull_hup  D13
//LED pin off
#define LED_OFF           PORTB &= ~(1<<PB5)                                 //Pull_down D13 

//Globale interrupt seting
#define GLOBAL_INTERRUPT_ENABLE SREG |= (1<<7)                                    //Set global interrupt enable SREG |=(1<<I)
#define GLOBAL_INTERRUPT_DISABLE SREG &= ~(1<<7)                                  //Set global interrupt disable SREG &= ~(1<<I)



// Handling pins for BIOS patch
//BIOS patch pins input
#define AX_INPUT        DDRD  &= ~(1<<DDD2)                             //INPUT     D2
#define AY_INPUT        DDRD  &= ~(1<<DDD3)                             //INPUT     D3
#define DX_INPUT        DDRD  &= ~(1<<DDD4)                             //INPUT     D4
//BIOS patch pin output
#define DX_OUTPUT       DDRD  |=  (1<<DDD4)                             //OUTPUT    D4
//Define pull-ups and set high at the BIOS patch pin
#define DX_SET          PORTD |=  (1<<PD4)                              //Pull_hup  D4
//Define pull-ups and set down at the BIOS patch pin
#define DX_CLEAR        PORTD &= ~(1<<PD4)                              //Pull_down D4
//Read pins for BIOS patch
#define AX_READ         (PIND & (1<<PIND2))                             //Read D2
#define AY_READ         (PIND & (1<<PIND3))                             //Read D3

void Init()
{
  TCNT0 = 0x00;                                                        
  OCR0A = 159;                                                         //OCR0A – Output Compare Register A, 0x10011111, 100KHz
  TCCR0A |= (1<<WGM01);                                                //TCCR0A – Timer/Counter Control Register A. turn on CTC mode, WGM01
  TCCR0B |= (1<<CS00);                                                 /*TCCR0B – Timer/Counter Control Register B,  CS00: Clock Select,  clk I/O
                                                                        | Waveform Generation Mode, Mode 2 CTC*/
  #ifdef PATCH_SW
   SW_USE;
  #endif

  LED_OUTPUT;
  GLOBAL_INTERRUPT_ENABLE;
  PIN_SQCK_INPUT;
  PIN_SUBQ_INPUT;                              
}






#endif
