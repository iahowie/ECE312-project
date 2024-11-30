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
