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
