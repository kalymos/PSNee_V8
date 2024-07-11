#pragma once

#ifdef ATmega328_168

#define F_CPU 16000000L
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

// Globale interrupt seting
#define GLOBAL_INTERRUPT_ENABLE     SREG   |=  (1<<7)           
#define GLOBAL_INTERRUPT_DISABLE    SREG   &= ~(1<<7)    

// Handling the main pins

// Main pins input
#define PIN_DATA_INPUT              DDRB   &= ~(1<<DDB0)        
#define PIN_WFCK_INPUT              DDRB   &= ~(1<<DDB1)       // Create a mask (1<<0) with the first bit at 1 b00000001 uses the ~ operator to perform a bit inversion b11111110,
#define PIN_SQCK_INPUT              DDRD   &= ~(1<<DDD6)       // &= updates the DDRB register with the AND operator and the mask, DDRB bxxxxxxxx OR mask b11111110 = bxxxxxxx0                     
#define PIN_SUBQ_INPUT              DDRD   &= ~(1<<DDD7) 
                            
// Main pin output
#define PIN_DATA_OUTPUT             DDRB   |=  (1<<DDB0)       // Create a mask (1<<0) with the first bit at 1 b00000001,     
#define PIN_WFCK_OUTPUT             DDRB   |=  (1<<DDB1)       // |= updates the DDRB register with the OR operator and the mask, DDRB bxxxxxxxx OR mask b00000001 = bxxxxxxx1
           
// Define pull-ups and set high at the main pin
#define PIN_DATA_SET                PORTB  |=  (1<<PB0)        // Create a mask (1<<0) with the first bit at 1 b00000001,
                                                               // |= updates the PORTB register with the OR operator and the mask, PORTB bxxxxxxxx OR mask b00000001 = bxxxxxxx1
             
// Define pull-ups set down at the main pin
#define PIN_DATA_CLEAR              PORTB  &= ~(1<<PB0)        // Create a mask (1<<0) with the first bit at 1 b00000001 uses the ~ operator to perform a bit inversion b11111110,                    
#define PIN_WFCK_CLEAR              PORTB  &= ~(1<<PB1)        // &= updates the DDRB register with the AND operator and the mask, DDRB bxxxxxxxx OR mask b11111110 = bxxxxxxx0
                         
// Read the main pins
#define PIN_SQCK_READ              (PIND   &   (1<<PIND6))     // Create a mask (1<<6) with the six bit at 1 b00100000,                       
#define PIN_SUBQ_READ              (PIND   &   (1<<PIND7))     // compare the PINB register and the mask with the AND operator, and returns the result, PINB bxx1xxxxx AND mask b00100000 = 1                      
#define PIN_WFCK_READ              (PINB   &   (1<<PINB1))                                                       

// Handling and use of the LED pin
#define LED_RUN
#define PIN_LED_OUTPUT              DDRB   |=  (1<<DDB5)                                
#define PIN_LED_ON                  PORTB  |=  (1<<PB5)      
#define PIN_LED_OFF                 PORTB  &= ~(1<<PB5)   

// Handling the BIOS patch

// BIOS interrupt seting
#define TIMER_INTERRUPT_ENABLE      TIMSK0 |=  (1<<OCIE0A)
#define TIMER_INTERRUPT_DISABLE     TIMSK0 &= ~(1<<OCIE0A)

// BIOS timer clear
#define TIMER_TCNT_CLEAR            TCNT0  =    0x00
#define TIMER_TIFR_CLEAR            TIFR0  |=  (1<<OCF0A)

// Pins input
#define PIN_AX_INPUT                DDRD   &= ~(1<<DDD2)                            
#define PIN_AY_INPUT                DDRD   &= ~(1<<DDD3)                             
#define PIN_DX_INPUT                DDRD   &= ~(1<<DDD4)                             
// Pin output
#define PIN_DX_OUTPUT               DDRD   |=  (1<<DDD4)                             
// Define pull-ups set high 
#define PIN_DX_SET                  PORTD  |=  (1<<PD4)                              
// Define pull-ups set down 
#define PIN_DX_CLEAR                PORTD  &= ~(1<<PD4)                            
// Read pins for BIOS patch
#define PIN_AX_READ                (PIND   &   (1<<PIND2))                             
#define PIN_AY_READ                (PIND   &   (1<<PIND3))                             

// Handling the interrupt
#define PIN_AX_INTERRUPT_ENABLE     EIMSK  |=  (1<<INT0)
#define PIN_AY_INTERRUPT_ENABLE     EIMSK  |=  (1<<INT1)

#define PIN_AX_INTERRUPT_DISABLE    EIMSK  &= ~(1<<INT0)
#define PIN_AY_INTERRUPT_DISABLE    EIMSK  &= ~(1<<INT1)

#define PIN_AX_INTERRUPT_RISING     EICRA  |=  (1<<ISC01)|(1<<ISC00)
#define PIN_AY_INTERRUPT_RISING     EICRA  |=  (1<<ISC11)|(1<<ISC10)

#define PIN_AX_INTERRUPT_FALLING    EICRA  |=  (1<<ISC01)
#define PIN_AY_INTERRUPT_FALLING    EICRA  |=  (1<<ISC11)

// Handling and reading the switch pin for patch BIOS
#define PIN_SWITCH_INPUT            DDRD   &= ~(1<<DDD5)                              
#define PIN_SWITCH_SET              PORTD  |=  (1<<PD5)                                
#define PIN_SWICHE_READ            (PIND   &   (1<<PIND5))

#endif
