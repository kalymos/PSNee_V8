#pragma once

#ifdef ATmega328_168

#define DATA_OUTPUT     DDRB |= (1<<0)                               //It is an equivalence to "pinMode (8, OUTPUT);" Create a mask (1<<0) with the first bit at 1 b00000001, |= updates the DDRB register with the OR operator and the mask, DDRB bxxxxxxxx OR mask b00000001 = bxxxxxxx1
#define DATA_INPUT      DDRB &= ~(1<<0)                              //It is an equivalence to "pinMode (8, INPUT);" Create a mask (1<<0) with the first bit at 1 b00000001 uses the ~ operator to perform a bit inversion b11111110 , &= updates the DDRB register with the AND operator and the mask, DDRB bxxxxxxxx OR mask b11111110 = bxxxxxxx0
#define DATA_CLEAR      PORTB &= ~(1<<0)                             //It is an equivalence to "digitalWrite(8, LOW);" Create a mask (1<<0) with the first bit at 1 b00000001 uses the ~ operator to perform a bit inversion b11111110 , &= updates the DDRB register with the AND operator and the mask, DDRB bxxxxxxxx OR mask b11111110 = bxxxxxxx0
#define DATA_SET        PORTB |= (1<<0)                              //It is an equivalence to "digitalWrite(8, HIGH))" Create a mask (1<<0) with the first bit at 1 b00000001, |= updates the PORTB register with the OR operator and the mask, PORTB bxxxxxxxx OR mask b00000001 = bxxxxxxx1
#define WFCK_OUTPUT     DDRB |= (1<<1)
#define WFCK_INPUT      DDRB &= ~(1<<1)
#define WFCK_CLEAR      PORTB &= ~(1<<1)
#define DX_OUTPUT       DDRD |= (1<<4)
#define DX_INPUT        DDRD &= ~(1<<4)
#define DX_CLEAR        PORTD &= ~(1<<4)
#define DX_SET          PORTD |= (1<<4)

#define SQCK_READ       (PIND & (1<<6))                             //It is an equivalence to "digitalRead(6)" Create a mask (1<<6) with the six bit at 1 b00100000, compare the PINB register and the mask with the AND operator, and returns the result, PINB bxx1xxxxx AND mask b00100000 = 1, 
#define SUBQ_READ       (PIND & (1<<7))
#define WFCK_READ       (PINB & (1<<1))
#define AX_READ         (PIND & (1<<2))
#define AY_READ         (PIND & (1<<3))

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

#define SW_USE			PORTD |= (1<<5)
#define SW_CHECK		(PIND & (1<<5))

#define LED_USE			DDRB |= (1<<5)
#define LED_ON			PORTB |= (1<<5)
#define LED_OFF			PORTB &= ~(1<<5)

void Init()
{
	TCNT0 = 0x00;
	OCR0A = 159; 
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS00);

	#ifdef PATCH_SW
	 SW_USE;
	#endif

	LED_USE;
	sei();
}

#endif
