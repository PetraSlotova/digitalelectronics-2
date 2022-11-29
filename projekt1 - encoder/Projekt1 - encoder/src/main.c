/***********************************************************************
 * 
 * Stopwatch by Timer/Counter2 on the Liquid Crystal Display (LCD)
 *
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2017 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 * Components:
 *   16x2 character LCD with parallel interface
 *     VSS  - GND (Power supply ground)
 *     VDD  - +5V (Positive power supply)
 *     Vo   - (Contrast)
 *     RS   - PB0 (Register Select: High for Data transfer, Low for Instruction transfer)
 *     RW   - GND (Read/Write signal: High for Read mode, Low for Write mode)
 *     E    - PB1 (Read/Write Enable: High for Read, falling edge writes data to LCD)
 *     D3:0 - NC (Data bits 3..0, Not Connected)
 *     D4   - PD4 (Data bit 4)
 *     D5   - PD5 (Data bit 5)
 *     D6   - PD6 (Data bit 6)
 *     D7   - PD7 (Data bit 7)
 *     A+K  - Back-light enabled/disabled by PB2
 * 
 **********************************************************************/

/* Define pins for joystick ------------------------------------------*/
#define Rx PC0 
#define Ry PC1 
#define SW PD2 

/* Define pins for rotary ---------------------------------------------*/
#define CLK PB3
#define DT PB4 
#define SW PB5 

#define SHORT_DELAY 5 // Delay in milliseconds

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <util/delay.h>     // Functions for busy-wait delay loops
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <lcd.h>            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for number conversions

/* Arduino world -----------------------------------------------------*/
#include "Arduino.h"
#define PC0 A0
#define PC1 A1
#define PD2 2
#define PB3 11
#define PB4 12
#define PB5 13

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Update stopwatch value on LCD screen when 8-bit 
 *           Timer/Counter2 overflows.
 * Returns:  none
 **********************************************************************/
// Define global variables for charakters and directions
uint16_t h = 0, v = 0; 
uint16_t ch = 1; 

int main(void)
{
    /* -------------------------Initialize display -----------------------------*/
    lcd_init(LCD_DISP_ON_CURSOR);
    
    /* -----------------------------Joystick -----------------------------------*/
    // Configure Analog-to-Digital Convertion unit
    // Select ADC voltage reference to "AVcc with external capacitor at AREF pin"
    ADMUX = ADMUX |  (1<<REFS0);
    // Enable ADC module
    ADCSRA |= (1<<ADEN); // into the variable ADCSRA counting a new value
    // Enable conversion complete interrupt
    ADCSRA |= (1<<ADIE);
    // Set clock prescaler to 128
    ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

    // Configure 16-bit Timer/Counter1 to start ADC conversion
    // Set prescaler to 33 ms and enable overflow interrupt
    TIM1_overflow_33ms();
    TIM1_overflow_interrupt_enable();
    
    PCICR |= (1<<PCIE0);
    PCMSK0 |= (1<<PCINT3);
    PCMSK0 |= (1<<PCINT4);

    EIMSK |= (1 << INT0);
    GPIO_mode_input_pullup(&DDRD, PD2);

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines, ISRs */
    }

    // Will never reach this
    return 0;

}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Use single conversion mode and start conversion every 100 ms.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    // Start ADC conversion
    ADCSRA |= (1<<ADSC);
}

ISR(PCINT0_vect)
{
    char string[4];  // String for converted numbers by itoa()
    _delay_ms(SHORT_DELAY);
    if (ch < 9)
    {
        ch++;
        lcd_gotoxy(h, v);
        itoa(ch, string, 10);
        lcd_puts(string);
    }
    //int clk = GPIO_read(&PINB, 3);
    //int direction = GPIO_read(&PINB, 4);
}  

ISR(INT0_vect)
{
    char string[4];  // String for converted numbers by itoa()
    uint8_t sw = digitalRead(2);
    if (sw == LOW)
    {
        ch = 1;
        lcd_gotoxy(h, v);
        itoa(ch, string, 10);
        lcd_puts(string);
    }
    
}

/**********************************************************************
 * Function: ADC complete interrupt
 * Purpose:  Display converted value on LCD screen.
 **********************************************************************/
ISR(ADC_vect)
{
    uint16_t x, y;
    char string[4];  // String for converted numbers by itoa()

    // Read x value ----------------------------------------------------------
    // Note that, register pair ADCH and ADCL can be read as a 16-bit value ADC
    // Select input channel ADC1 (voltage divider pin)
    uint8_t channel = ADMUX & 0b00001111;
    if (channel == 0)
    {
    x = ADC;

    if ((x<411)|(x>611))
    {
        // Convert "value" to "string" and display it

        if(x<411)
        {
            lcd_gotoxy(h, v);
            lcd_puts(" ");
            if (v == 1){
                v--;
            }
            lcd_gotoxy(h, v);
            itoa(ch, string, 10);
            lcd_puts(string);
        }
        else if(x>611)
        {
            lcd_gotoxy(h, v);
            lcd_puts(" ");
            if (v == 0){
                v++;
            }
            lcd_gotoxy(h, v);
            itoa(ch, string, 10);
            lcd_puts(string);
        }
    }
    ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1); ADMUX |= (1<<MUX0) ;
    }

    else if (channel == 1)
    {
        // Read y value ----------------------------------------------------------
        // Note that, register pair ADCH and ADCL can be read as a 16-bit value ADC
        y = ADC;

        if(y<411)
        {
            lcd_gotoxy(h, v);
            lcd_puts(" ");
            if (h < 15){
                h++;
            }
            lcd_gotoxy(h, v);
            itoa(ch, string, 10);
            lcd_puts(string);
        }
        else if(y>611)
        {
            lcd_gotoxy(h, v);
            lcd_puts(" ");
            if (h > 0){
                h--;
            }
            lcd_gotoxy(h, v);
            itoa(ch, string, 10);
            lcd_puts(string);
        }
        ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<<MUX0);
        }
}