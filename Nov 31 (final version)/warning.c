#define F_CPU 1000000UL // Define clock speed

#include <avr/io.h>
#include <util/delay.h>
#include "warning.h"

#define SERVO PD3     // OC2B pin (Servo control on Pin 5)
#define BUTTON PD7    // Button pin for toggling
#define LED PB0       // LED pin
#define SPEAKER PD6   // Speaker pin (OC0A)

void warning_routine_inializatoin() {
    DDRD |= (1 << SPEAKER); // Set PD6 (OC0A) as output
    TCCR0A = (1 << WGM01) | (1 << WGM00); // Fast PWM mode
    TCCR0B = (1 << CS01);  // Prescaler = 8
    
    // initialize LED
    DDRB |= (1 << LED);    // Set LED pin as output
    DDRD &= ~(1 << BUTTON); // Set button pin as input
    PORTD |= (1 << BUTTON); // Enable pull-up resistor for button
    
    
    // initialize PWM for the speaker
    DDRD |= (1 << SPEAKER); // Set PD6 (OC0A) as output
    TCCR0A = (1 << WGM01) | (1 << WGM00); // Fast PWM mode
    TCCR0B = (1 << CS01);  // Prescaler = 8  
    
    
    // initialize PWM for the servo
    DDRD |= (1 << SERVO); // Set PD3 (OC2B) as output
    // Configure Timer2 for Phase Correct PWM
    TCCR2A = (1 << COM2B1) | (1 << WGM20);  // Phase Correct PWM, non-inverted
    TCCR2B = (1 << WGM22) | (1 << CS22) | (1 << CS20); // Prescaler = 128
    // Set TOP value for 50 Hz (20 ms period)
    OCR2A = 79;
    // Initialize duty cycle to 0%
    OCR2B = 0;       
}


void start_warning_routine(){    
    // Blink LED and toggle speaker at the same frequency
    set_duty_cycle(3);
    PORTB ^= (1 << LED); // Toggle LED
    play_speaker();      // Play speaker at 1 kHz
    _delay_ms(500);          // Half "on" time
    stop_speaker();          // Stop speaker
    _delay_ms(500);          // Half "off" time
}
void end_warning_routine(){
    PORTB &= ~(1 << LED); // Ensure LED is off
    stop_speaker();          // Ensure speaker is off
    set_duty_cycle(8);
}

void set_duty_cycle(uint8_t dutyCycle) {
    // Convert duty cycle (0-100%) to OCR2B value
    OCR2B = (dutyCycle * 79) / 100; // Scale duty cycle to TOP value
}


// Function to play the speaker with a given frequency
void play_speaker( ) {
    TCCR0A |= (1 << COM0A1); // Enable PWM output on OC0A (PD6)
    OCR0A = 124;
}

// Function to stop the speaker
void stop_speaker() {
    TCCR0A &= ~(1 << COM0A1); // Disable PWM output on OC0A (PD6)
    OCR0A = 0;
    
}


