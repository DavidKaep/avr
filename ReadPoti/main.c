/*
Title: blinking_led.c
Author: you
Date Created: 01/01/01
Last Modified: 01/01/01
Purpose: This is a for loop that blinks an LED on PORTC
*/
//set your clock speed
#define	F_CPU	1000000UL
#define LTHRES 500
#define RTHRES 500
//these are the include files. They are outside the project folder
#include <avr/io.h>
#include <avr/interrupt.h> 
#include <stdio.h>
#include <stdint.h>
//this include is in your project folder

#define F_CPU 16000000UL
#define BAUD 9600

#include <util/setbaud.h>

// global variable to count the number of overflows
volatile uint8_t tot_overflow;
  
// initialize timer, interrupt and variable
void timer1_init()
{
    // set up timer with prescaler = 8
    TCCR1B |= (1 << CS11);
  
    // initialize counter
    TCNT1 = 0;
  
    // enable overflow interrupt
    TIMSK1 |= (1 << TOIE1);
  
    // enable global interrupts
    sei();
  
    // initialize overflow counter variable
    tot_overflow = 0;
}
  
// initialize adc
void adc_init()
{
    // AREF = AVcc
    ADMUX = (1<<REFS0);
 
    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}
 
// read adc value
uint16_t adc_read(uint8_t ch)
{
    // select the corresponding channel 0~7
    // ANDing with '7' will always keep the value
    // of 'ch' between 0 and 7
    ch &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8)|ch;     // clears the bottom 3 bits before ORing
 
    // start single conversion
    // write '1' to ADSC
    ADCSRA |= (1<<ADSC);
 
    // wait for conversion to complete
    // ADSC becomes '0' again
    // till then, run loop continuously
    while(ADCSRA & (1<<ADSC));
 
    return (ADC);
}


volatile uint16_t readerValue=0;
// TIMER1 overflow interrupt service routine
// called whenever TCNT1 overflows
ISR(TIMER1_OVF_vect)
{
    // keep a track of number of overflows
    tot_overflow++;
  
    // check for number of overflows here itself
    // 61 overflows = 2 seconds delay (approx.)
    if (tot_overflow >= 5) // NOTE: '>=' used instead of '=='
   {
        //PORTD ^= 0b11110000;  // toggles the led
        // no timer reset required here as the timer
        // is reset every time it overflows
 	readerValue = adc_read(0);
	printf("%d\n", readerValue/11);
        tot_overflow = 0;   // reset overflow counter
    }
}

void uart_putchar(char c, FILE *stream) {
    if (c == '\n') {
        uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

char uart_getchar(FILE *stream) {
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
    return UDR0;
}

void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);



int main(void)
{
    uart_init();
    adc_init();

    stdout = &uart_output;
    stdin  = &uart_input;
    // initialize timer
    timer1_init();
    sei(); 
    // loop forever
    while(1)
    {
        // do nothing
        // comparison is done in the ISR itself
    }
}
