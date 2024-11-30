l4z3r5h4rk
l4z3r5h4rk
Idle
ECE312 Project

Adri — Yesterday at 20:32
okay so what do you think we should do
the mp3file thing
how?
l4z3r5h4rk — Yesterday at 20:32
Idk lol
Image
```An ATmega can play a mono WAV off a SD card but it'll literally hog all your CPU cycles, leaving no execution time for anything else.

An ATmega can play MP3 if it is used in conjunction with a VS10xx decoder and still have some execution time left for other tasks.

Something like a 160MHz ARM Cortex can definitely decode MP3s and use internal DACs to play sound and not even sweat doing it

Adafruit makes a bunch of different audio modules, built-in card reader and dedicated decoder, even amplifier, with a parallel or serial control. I would recommend this for your needs unless cost is an issue
Too bad we cant use stm32 from 212
ministerhonda — Yesterday at 20:44
Adri — Yesterday at 21:14
Adri — Yesterday at 21:25
ministerhonda pinned a message to this channel. See all pinned messages. — Today at 0:01
ministerhonda pinned a message to this channel. See all pinned messages. — Today at 0:01
Fuzzabra — Today at 12:01
Anyone in the lab yet?
Adri — Today at 12:19
Im at ecerf
Are you in 312 lab room?
l4z3r5h4rk — Today at 12:20
ill be there in half an hour
Fuzzabra — Today at 12:25
Nah im in Rutherford rn
I thought I’d start walking over if anyone’s there yet
Adri — Today at 12:29
yeah i am here
Fuzzabra — Today at 12:29
Sweet
I’ll be there in 15
Adri — Today at 12:30
ok
ministerhonda — Today at 12:35
I'm in ecerf
Where are you
Adri — Today at 12:39
312 room
ministerhonda — Today at 12:43
K
ministerhonda — Today at 13:38
@l4z3r5h4rk
are you coming today lol
l4z3r5h4rk — Today at 13:38
Yeah im coming
I was just applying to summer jobs
Took way longer than expected
l4z3r5h4rk — Today at 13:55
Wya
Im in etlc
ministerhonda — Today at 13:55
Image
ministerhonda — Today at 13:55
come to the lab room lol
ministerhonda — Today at 14:13
Image
l4z3r5h4rk — Today at 14:16
nice
ministerhonda — Today at 14:54
oops this was the wrong one
what a sick life
Image
here is the right one
ministerhonda — Today at 15:02
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
... (57 lines left)
Collapse
message.txt
6 KB
Fuzzabra — Today at 15:13
Forwarded
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SERVO_PIN PB1 // OC1A pin (Pin 9 on ATmega328P)

void setupPWM() {
    // Set the servo control pin as output
    DDRB |= (1 << SERVO_PIN);
    
    // Set Timer 1 for Fast PWM mode with non-inverted output
    TCCR1A = (1 << COM1A1) | (1 << WGM11) | (1 << WGM10); // Fast PWM, 8-bit mode
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);   // Prescaler of 8

    // Set the TOP value for 50Hz frequency (PWM period = 20ms)
    ICR1 = 19999; // 16MHz clock / (8 prescaler * 50Hz) - 1 = 19999
}

void setServoAngle(uint8_t angle) {
    // Map the angle (0-180 degrees) to a pulse width (1000 to 2000 microseconds)
    uint16_t pulseWidth = (angle * 1000) / 180 + 1000;
    
    // Set the compare value for the servo pulse width
    OCR1A = pulseWidth * 2; // Map microseconds to Timer ticks (16 MHz clock with prescaler of 8)
}

int main() {
    setupPWM();
    
    while (1) {
        for (uint8_t angle = 0; angle <= 180; angle++) {
            setServoAngle(angle);
            _delay_ms(15); // Wait for the servo to move
        }
        
        for (uint8_t angle = 180; angle >= 0; angle--) {
            setServoAngle(angle);
            _delay_ms(15); // Wait for the servo to move
        }
    }
}
Adri — Today at 15:21
#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL // Define clock speed

// Function to check if the button is pressed (debounced)
Expand
message.txt
3 KB
Fuzzabra — Today at 15:35
Forwarded
#include <avr/io.h>
#include <util/delay.h>

// Define pulse widths for servo angles
#define SERVO_MIN 1000  // 1ms pulse width
#define SERVO_MAX 2000  // 2ms pulse width
#define SERVO_MID 1500  // 1.5ms pulse width (90 degrees)

void Timer1_Init() {
    // Set PB1/OC1A as output
    DDRB |= (1 << PB1);

    // Configure Timer1 for Fast PWM, non-inverted mode
    TCCR1A = (1 << WGM11) | (1 << COM1A1);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler = 8

    // Set TOP value for 20ms period (50Hz)
    ICR1 = 39999; // F_CPU = 16MHz, Prescaler = 8 -> (16,000,000 / 8) / 50 - 1

    // Initialize servo to mid position
    OCR1A = SERVO_MID;
}

void Servo_SetAngle(uint16_t pulse_width) {
    if (pulse_width < SERVO_MIN) pulse_width = SERVO_MIN;
    if (pulse_width > SERVO_MAX) pulse_width = SERVO_MAX;

    OCR1A = (pulse_width * 2); // Scale to match ICR1 ticks
}

int main() {
    Timer1_Init();

    while (1) {
        // Simulate lowering the barrier
        Servo_SetAngle(SERVO_MAX); // Move to 180 degrees
        _delay_ms(3000);           // Simulate train in range

        // Simulate raising the barrier
        Servo_SetAngle(SERVO_MIN); // Move to 0 degrees
        _delay_ms(3000);           // Simulate train out of range
    }
}
l4z3r5h4rk — Today at 16:34
https://github.com/iahowie/ECE312-project
GitHub
GitHub - iahowie/ECE312-project
Contribute to iahowie/ECE312-project development by creating an account on GitHub.
GitHub - iahowie/ECE312-project
github repo for our project
ministerhonda — Today at 16:36
@Fuzzabra
Attachment file type: acrobat
Lecture 15 serial com.pdf
1.89 MB
Attachment file type: acrobat
Lecture 17 spi.pdf
1.43 MB
Attachment file type: acrobat
Lecture 18 synchronous serial.pdf
1.62 MB
Attachment file type: acrobat
Lecture 19 avr adc.pdf
2.19 MB
Attachment file type: acrobat
Lecture 20 real time.pdf
1.40 MB
Adri — Today at 16:38
#include <avr/io.h>
#include <util/delay.h>

#define SERVO_PIN PD3  // OC2B pin (Pin 5 on ATmega328P)

void setupPWM() {
    // Set the servo control pin as output
    DDRD |= (1 << SERVO_PIN);

    // Configure Timer 2 for Phase Correct PWM mode
    TCCR2A = (1 << COM2B1) | (1 << WGM20);  // Phase Correct PWM, non-inverted
    TCCR2B = (1 << WGM22) | (1 << CS21);    // Phase Correct with prescaler of 8

    // Set the TOP value for 50Hz frequency (PWM period = 20ms)
    OCR2A = 599;  // TOP = (16MHz / (8 * 50Hz)) - 1 = 249
}

void setServoAngle(uint8_t angle) {
    // Map the angle (0-180 degrees) to a pulse width (1000 to 2000 microseconds)
    uint16_t pulseWidth = (angle * 1000) / 180 + 1000;

    // Convert pulse width to timer ticks
    uint8_t ticks = (pulseWidth - 1000) / 8;  // Scale to Timer 2 ticks (1 tick = 0.5us with prescaler of 8)
    OCR2B = ticks;  // Set the compare match value for OC2B
}

int main() {
    setupPWM();

    while (1) {
        // Sweep from 0° to 120° (approximate full range for HS-311)
        for (uint8_t angle = 0; angle <= 120; angle += 1) {
            setServoAngle(angle);
            _delay_ms(1);  // Delay for smooth movement
        }

        // Sweep back from 120° to 0°
        for (uint8_t angle = 120; angle > 0; angle -= 1) {
            setServoAngle(angle);
            _delay_ms(1);  // Delay for smooth movement
        }
    }
}
﻿
#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL // Define clock speed

// Function to check if the button is pressed (debounced)
int pressed() {
    if (!(PIND & (1 << PD2))) { // Button is active low
        _delay_ms(50);          // Debounce delay
        if (!(PIND & (1 << PD2))) { // Check again
            while (!(PIND & (1 << PD2))); // Wait for button release
            return 1; // Button press detected
        }
    }
    return 0; // Button not pressed
}

// Function to initialize Timer0 for PWM (Speaker)
void init_pwm() {
    DDRD |= (1 << PD6); // Set PD6 (OC0A) as output
    TCCR0A = (1 << WGM01) | (1 << WGM00); // Fast PWM mode
    TCCR0B = (1 << CS01) | (1 << CS00);   // Prescaler = 64
}

// Function to play the speaker with a given frequency
void play_speaker(uint16_t frequency) {
    // Calculate OCR0A for the given frequency
    OCR0A = (F_CPU / (64 * frequency)) - 1;
    TCCR0A |= (1 << COM0A1); // Enable PWM output on OC0A (PD6)
}

// Function to stop the speaker
void stop_speaker() {
    TCCR0A &= ~(1 << COM0A1); // Disable PWM output on OC0A (PD6)
    OCR0A = 0;                // Clear OCR0A to ensure no residual PWM
}

int main(void) {
    // Configure PB1 as an output for the LED
    DDRB |= (1 << PD1);

    // Configure PD2 as an input for the button
    DDRD &= ~(1 << PD2); // Set PD2 as input
    PORTD |= (1 << PD2); // Enable internal pull-up resistor on PD2

    // Initialize PWM for the speaker
    init_pwm();

    int blinking = 0; // Variable to track LED state: 0 = off, 1 = blinking

    while (1) {
        if (pressed()) {
            // Toggle the blinking state when the button is pressed
            blinking = !blinking;

            if (blinking) {
                play_speaker(10000); // Start the speaker at 1 kHz tone
            } else {
                stop_speaker();     // Stop the speaker
            }
        }

        if (blinking) {
            // If blinking is enabled, toggle the LED on and off
            PORTB ^= (1 << PD1); // Toggle LED
            _delay_ms(200);      // Delay for blinking
        } else {
            // If blinking is disabled, ensure the LED is off
            PORTB &= ~(1 << PD1); // Turn off LED
        }
    }

    return 0;
}
message.txt
3 KB
