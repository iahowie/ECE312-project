#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 14745600UL // Clock frequency
#define v_sound 34300    // Speed of sound in cm/s
#define DETECTION_THRESHOLD 5.0 // Threshold for detecting an incoming object (cm)

// U3 (Sensor 1)
#define trig1_pin PC4
#define echo1_pin PC5

// U5 (Sensor 2)
#define trig2_pin PB1
#define echo2_pin PB2

// Global variables for distances and timer counts
volatile uint16_t t0_1 = 0, t1_1 = 0; // Timer counts for sensor 1
volatile uint16_t t0_2 = 0, t1_2 = 0; // Timer counts for sensor 2
volatile float dist1 = 0, dist2 = 0;  // Current distances
volatile float prev_dist1 = 0, prev_dist2 = 0; // Previous distances
volatile uint8_t measuring1 = 0, measuring2 = 0; // Flags for measurement in progress

// ISR for Echo1 (PCINT1_vect)
ISR(PCINT1_vect) {
    if (PINC & (1 << echo1_pin)) {
        // Rising edge detected
        t0_1 = TCNT1; // Capture the current timer count
        measuring1 = 1; // Measurement in progress
    } else {
        // Falling edge detected
        t1_1 = TCNT1; // Capture the current timer count
        measuring1 = 0; // Measurement complete
    }
}

// ISR for Echo2 (PCINT0_vect)
ISR(PCINT0_vect) {
    if (PINB & (1 << echo2_pin)) {
        // Rising edge detected
        t0_2 = TCNT1; // Capture the current timer count
        measuring2 = 1; // Measurement in progress
    } else {
        // Falling edge detected
        t1_2 = TCNT1; // Capture the current timer count
        measuring2 = 0; // Measurement complete
    }
}

// Timer overflow interrupt (to handle potential overflow)
ISR(TIMER1_OVF_vect) {
    // Reset timer counts in case of overflow
    t0_1 = t1_1 = 0;
    t0_2 = t1_2 = 0;
}

// Function to trigger an ultrasonic pulse
void trigger_Pulse(uint8_t trig_pin) {
    if (trig_pin == trig1_pin) {
        PORTC |= (1 << trig1_pin); // Set PC4 high
        _delay_us(10);             // 10 µs pulse
        PORTC &= ~(1 << trig1_pin); // Set PC4 low
    } else if (trig_pin == trig2_pin) {
        PORTB |= (1 << trig2_pin); // Set PB1 high
        _delay_us(10);             // 10 µs pulse
        PORTB &= ~(1 << trig2_pin); // Set PB1 low
    }
}

// Function to calculate distance based on timer counts
float calculate_Distance(uint16_t t0, uint16_t t1) {
    uint32_t timer_count;

    if (t1 >= t0) {
        timer_count = t1 - t0; // Normal case
    } else {
        timer_count = (65536 + t1) - t0; // Handle timer overflow
    }

    return (0.5 * timer_count * v_sound) / F_CPU; // Calculate distance in cm
}

// Function to check for incoming objects
void detect_Incoming_Object(FILE *lcd_str) {
    // Left Sensor (U3)
    if (prev_dist1 > 0 && dist1 > 0 && (prev_dist1 - dist1) > DETECTION_THRESHOLD) {
        fprintf(lcd_str, "\x1b\x01"); // Clear LCD
        fprintf(lcd_str, "Incoming Left!");
        _delay_ms(500);
    }

    // Right Sensor (U5)
    if (prev_dist2 > 0 && dist2 > 0 && (prev_dist2 - dist2) > DETECTION_THRESHOLD) {
        fprintf(lcd_str, "\x1b\x01"); // Clear LCD
        fprintf(lcd_str, "Incoming Right!");
        _delay_ms(500);
    }
}

int main() {
    // Configure trig pins as outputs
    DDRC |= (1 << trig1_pin); // PC4
    DDRB |= (1 << trig2_pin); // PB1

    // Configure echo pins as inputs
    DDRC &= ~(1 << echo1_pin); // PC5
    DDRB &= ~(1 << echo2_pin); // PB2

    // Enable pin change interrupts for PC5 (echo1) and PB2 (echo2)
    PCICR |= (1 << PCIE1) | (1 << PCIE0);
    PCMSK1 |= (1 << PCINT13); // Enable interrupt for PC5
    PCMSK0 |= (1 << PCINT2);  // Enable interrupt for PB2

    // Configure Timer1
    TCCR1A = 0x00;          // Normal mode
    TCCR1B |= (1 << CS10);  // No prescaler
    TIMSK1 |= (1 << TOIE1); // Enable timer overflow interrupt

    // Configure LCD
    FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
    lcd_init();

    sei(); // Enable global interrupts

    while (1) {
        // Trigger pulses for both sensors
        trigger_Pulse(trig1_pin);
        _delay_ms(50); // Small delay to avoid interference
        trigger_Pulse(trig2_pin);

        // Calculate distances if measurements are complete
        if (!measuring1) {
            prev_dist1 = dist1; // Store previous distance
            dist1 = calculate_Distance(t0_1, t1_1);
        }
        if (!measuring2) {
            prev_dist2 = dist2; // Store previous distance
            dist2 = calculate_Distance(t0_2, t1_2);
        }

        // Detect incoming objects
        detect_Incoming_Object(&lcd_str);

        // Debug: Display distances on the LCD
        fprintf(&lcd_str, "\x1b\x01"); // Clear LCD
        fprintf(&lcd_str, "L: %.1f cm", dist1); // Display distance for sensor 1
        fprintf(&lcd_str, "\x1b\xc0"); // Move to second line
        fprintf(&lcd_str, "R: %.1f cm", dist2); // Display distance for sensor 2

        _delay_ms(500); // Delay before the next measurement cycle
    }

    return 0;
}
