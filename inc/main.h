#ifndef __MAIN_H
#define __MAIN_H

#include "stm8s.h"
#include "delay.h"


#define EV1527_PIN 	GPIO_PIN_3
#define EV1527_PORT GPIOC

#define LED_PIN		GPIO_PIN_3
#define LED_PORT	GPIOD

#define Sync_Min_width  2300
#define Sync_Max_width  3000
#define Noise_threshold 70 // use this carefully


//Pressed Key Code
enum KeyCodes{
    A_key  = 0b0001, 
    B_key  = 0b0010, 
    C_key  = 0b0100, 
    D_key  = 0b1000, 
    AB_key = 0b0011, 
    AC_key = 0b0101, 
    AD_key = 0b1001, 
    BC_key = 0b0110, 
    BD_Key = 0b1010, 
    CD_key = 0b1100
};

@svlreg INTERRUPT void EXTI_PORTC_IRQHandler(void); /* EXTI PORTC */

#endif  /* __MAIN_H */