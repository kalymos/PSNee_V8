extern volatile uint8_t count_isr;
extern volatile uint32_t microsec;
extern volatile uint16_t millisec;



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

// Interrupt Service Routine (ISR) for TIMER0 compare match A vector
ISR(TIMER0_COMPA_vect)
{
  microsec += 10;                                           // Increment microseconds by 10 for timing purposes
  count_isr++;                                              // Increment ISR count
  if (count_isr == 100)                                     // If ISR count reaches 100, increment milliseconds and reset ISR count
  {
    millisec++;
    count_isr = 0;
  }
}

void Timer_Start()
{
  TIMER_TCNT_CLEAR;
  TIMER_TIFR_CLEAR;
  TIMER_INTERRUPT_ENABLE;
}

void Timer_Stop()
{
  TIMER_INTERRUPT_DISABLE;
  TIMER_TCNT_CLEAR;
  count_isr = 0;
  microsec = 0;
  millisec = 0;
}
