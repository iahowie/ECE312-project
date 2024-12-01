#include <avr/io.h>
#include <util/delay.h>

#define SERVO_PIN PD3   // OC2B pin (Pin 5 on ATmega328P)
#define BUTTON_PIN PD2  // Button pin for toggling direction

void setupPWM() {
    // Set the servo control pin as output
    DDRD |= (1 << SERVO_PIN);

    // Configure Timer 2 for Phase Correct PWM mode
    TCCR2A = (1 << COM2B1) | (1 << WGM20);  // Phase Correct PWM, non-inverted
    TCCR2B = (1 << WGM22) | (1 << CS21);    // Phase Correct with prescaler of 8

    // Set TOP value for 50Hz frequency (20ms period)
    OCR2A = 249;  // TOP = (16MHz / (8 * 50Hz)) - 1 = 249
}

void setupButton() {
    // Set the button pin as input with pull-up resistor
    DDRD &= ~(1 << BUTTON_PIN);  // Set PD2 as input
    PORTD |= (1 << BUTTON_PIN);  // Enable pull-up resistor
}

uint8_t isButtonPressed() {
    return !(PIND & (1 << BUTTON_PIN));  // Check if button is pressed (active low)
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
    setupButton();

    int angle = 90;           // Start at 90°
    int direction = -1;       // Initial direction: down (-1 for down, +1 for up)

    while (1) {
        // Check if the button is pressed to toggle direction
        if (isButtonPressed()) {
            _delay_ms(200);  // Debounce delay
            direction = -direction;  // Toggle direction
        }

        // Move the servo in the current direction
        angle += direction;
        if (angle > 90) angle = 90;  // Limit to 90° (up)
        if (angle < 0) angle = 0;    // Limit to 0° (down)

        setServoAngle(angle);
        _delay_ms(15);  // Smooth movement delay
    }
}
