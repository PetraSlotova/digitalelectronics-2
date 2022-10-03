
#define LED_GREEN 13  
#define SHORT_DELAY 250 
#define LONG_DELAY 500 
#ifndef F_CPU
# define F_CPU 16000000 
#endif

#include <avr/io.h>    
#include <util/delay.h> 

#include "Arduino.h"        


int main(void)
{
    // Set pin where on-board LED is connected as output
    pinMode(LED_GREEN, OUTPUT);

    // Infinite loop
    while (1)
    {
        // Generate a lettre `A` Morse code

        // WRITE YOUR CODE HERE
        digitalWrite(LED_GREEN, LOW);
        _delay_ms(SHORT_DELAY);
        digitalWrite(LED_GREEN, HIGH);
        _delay_ms(SHORT_DELAY);
        digitalWrite(LED_GREEN, LOW);
        _delay_ms(SHORT_DELAY);
        digitalWrite(LED_GREEN, HIGH);
        _delay_ms(LONG_DELAY);

    }

    // Will never reach this
    return 0;
}