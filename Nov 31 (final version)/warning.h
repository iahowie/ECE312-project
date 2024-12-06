#ifndef WARNING_ROUTINE_H
#define	WARNING_ROUTINE_H

#include <avr/io.h>
#include <stdint.h>

//Pin definitions
#define SERVO PD3    
#define BUTTON PD7    
#define LED PB0      
#define SPEAKER PD6   

//Function prototypes
void warning_routine_inializatoin(); 
void start_warning_routine();    
void end_warning_routine();      
void set_duty_cycle(uint8_t dutyCycle); 
void play_speaker();               
void stop_speaker();                

#endif //WARNING_ROUTINE_H

