#include <avr/io.h>
#include <avr/interrupt.h>
 
void timer0_init()
{
   // Set on match from top, clear on match from below
   TCCR0A  = ((1 << COM0A1) | (1 << WGM00) | (1 << COM0B1) | (1 << COM0B0));
   TCCR1A = ((1 << COM1A1) | (1 << COM1A0) | (1 << WGM10));

   // prescaler
   TCCR1B = (1 << CS10);
   TCCR0B  = (1 << CS00);
}
 
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

int main(void)
{
	sei();
    adc_init();
    // connect led to pin PC0
    DDRD |= ((1 << 6) | (1 << 5) );
    DDRB |= (1 << 1);
 
    // initialize timer
    timer0_init();
    uint8_t value;
    // loop forever
    while(1){
    //set pwm to poti value
    value = adc_read(0)/4;
    OCR1A = value;
    OCR0B = adc_read(1)/4;
    OCR0A = value; 
    }
}
