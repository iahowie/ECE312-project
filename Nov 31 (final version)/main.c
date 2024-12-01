#define F_CPU 1000000UL // Define clock speed

#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "warning.h"

/* constants */

//U3, on PC pins
#define u3_trig_pin PC4
#define u3_echo_pin PC5

//U5, on PB pins 
#define u5_trig_pin PB1
#define u5_echo_pin PB2

#define WARNING_THRESHOLD 15.0


const float time_per_tick_us = 1.0;

//sound velocity cm/us
const float sound_speed = 0.0343;

//maximum counter ticks 2^16
const uint32_t max_ctr = 65536UL;

/*global vars*/

int warning = 0;

//sensor rising falling buffers 
volatile uint16_t u3_rising = 0;
volatile uint16_t u5_rising = 0;
volatile uint16_t u3_falling = 0;
volatile uint16_t u5_falling = 0;

//state of pulse 1 -> rising 0 -> falling
volatile uint8_t u3_edge = 1;
volatile uint8_t u5_edge = 1;

//overflow counters used in interrupt
volatile uint16_t u3_overflow_ctr = 0;
volatile uint16_t u5_overflow_ctr = 0;

//waits for a complete measurement
volatile uint8_t u3_new_meas = 0;
volatile uint8_t u5_new_meas = 0;

//distances
float u3_distance_cm = 0;
float u5_distance_cm = 0;

//local vars
int16_t u3_lf = 0;
int16_t u3_lr = 0;

int16_t u5_lf = 0;
int16_t u5_lr = 0;

int16_t u3_lof_ctr = 0;
int16_t u5_lof_ctr = 0;

//U3
ISR(PCINT1_vect) {
    
    if (u3_edge) {
        
        u3_overflow_ctr = 0;
        u3_rising = TCNT1;
        u3_edge = 0; 
        
    } else {
        
        u3_falling = TCNT1;
        u3_edge = 1;
        u3_new_meas = 1;
        
    }
}

//U5
ISR(PCINT0_vect) {
    
    if (u5_edge) {
        
        u5_overflow_ctr = 0; 
        u5_rising = TCNT1;
        u5_edge = 0; 
        
    } else {
        
        u5_falling = TCNT1;
        u5_edge = 1;
        u5_new_meas = 1;
    }
}

//timer overflow
ISR(TIMER1_OVF_vect) {
    u3_overflow_ctr ++;
    u5_overflow_ctr ++;
}

/* Function definitions */

//pin should be the trigger pin
void send_pulse(volatile uint8_t *port, uint8_t pin) {
    *port &= ~(1 << pin);
    _delay_us(5);
    *port |= (1 << pin); 
    _delay_us(10);
    *port &= ~(1 << pin);
}

//ticks to centimeters
float ticks_to_centi(int32_t re, int32_t fe, uint8_t of){
    
    float local_time_us = 0;
    float local_distance_cm = 0;
    
    int16_t local_delta = fe - re;
    uint16_t local_total_ticks = (of * max_ctr) + local_delta;
    
    local_time_us = local_delta * time_per_tick_us;
    local_distance_cm = (local_time_us * sound_speed) / 2.0f;
    
    return local_distance_cm;
}

int main() 
{
    warning_routine_inializatoin();
    /* Ports and IO */
    
    //enable echo pins (input))
    DDRC &= ~(1 << u3_echo_pin);
    DDRB &= ~(1 << u5_echo_pin);
    
    //enable trig pins (output)
    DDRC |= (1 << u3_trig_pin);
    DDRB |= (1 << u5_trig_pin); 
    
    /* rising and falling edge detection */
    
    //enables pin change interrupts for PC and PB 
    PCICR |= (1 << PCIE1) | (1 << PCIE0); 
    
    //masks echo 1
    PCMSK1 |= (1 << PCINT13);
    
    //masks echo 2
    PCMSK0 |= (1 << PCINT2); 
    
    /* timer clock and prescaler */
    
    TCCR1A = 0x00;
    
    //clock enable, no prescaler
    TCCR1B |= (1 << CS10);

    //enable timer overflow interrupt
    TIMSK1 |= (1 << TOIE1);

    sei(); //enable global interrupts
        
    //configures lcd library
    FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
    lcd_init();
            
    while (1) {
               
        //u3 pulse and measurement
        send_pulse(&PORTC, u3_trig_pin);
        
        while (u3_new_meas == 0) {}
        
        cli();
        
        u3_lf = u3_falling;
        u3_lr = u3_rising;

        u3_lof_ctr = u3_overflow_ctr;
       
        u3_new_meas = 0;
        
        sei();
        
        u3_distance_cm = ticks_to_centi(u3_lr, u3_lf, u3_lof_ctr);
        
        //u5
        send_pulse(&PORTB, u5_trig_pin);
        
        while (u5_new_meas == 0) {}
        
        //disable interrupts while working with volatile variables
        cli();

        u5_lf = u5_falling;
        u5_lr = u5_rising;

        u5_lof_ctr = u5_overflow_ctr;

        u5_new_meas = 0;

        sei();
        
        u5_distance_cm = ticks_to_centi(u5_lr, u5_lf, u5_lof_ctr);
                       
        fprintf(&lcd_str, "\x1b\x01"); // clears lcd
        int integer_part = (int)u3_distance_cm;
        int decimal_part = (int)((u3_distance_cm - integer_part) * 10); // Extract one decimal place
        fprintf(&lcd_str, "U3: %d.%d cm", integer_part, decimal_part);

        fprintf(&lcd_str, "\x1b\xc0"); // Move to second line
        
        integer_part = (int)u5_distance_cm;
        decimal_part = (int)((u5_distance_cm - integer_part) * 10);
        fprintf(&lcd_str, "U5: %d.%d cm", integer_part, decimal_part);
        
        while ((u3_distance_cm < WARNING_THRESHOLD) || (u5_distance_cm < WARNING_THRESHOLD)) {
            start_warning_routine();

            // Update u3 distance
            send_pulse(&PORTC, u3_trig_pin);
            while (u3_new_meas == 0) {}
            cli();
            u3_lf = u3_falling;
            u3_lr = u3_rising;
            u3_lof_ctr = u3_overflow_ctr;
            u3_new_meas = 0;
            sei();
            u3_distance_cm = ticks_to_centi(u3_lr, u3_lf, u3_lof_ctr);

            // Update u5 distance
            send_pulse(&PORTB, u5_trig_pin);
            while (u5_new_meas == 0) {}
            cli();
            u5_lf = u5_falling;
            u5_lr = u5_rising;
            u5_lof_ctr = u5_overflow_ctr;
            u5_new_meas = 0;
            sei();
            u5_distance_cm = ticks_to_centi(u5_lr, u5_lf, u5_lof_ctr);
        }

        end_warning_routine();
        
        _delay_ms(250);
    }
    
    return 0;
}