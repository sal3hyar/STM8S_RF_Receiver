#ifndef __SERIAL_H
#define __AERIAL_H

#include "stm8s.h"

void Serial_new_line(void);
void Serial_print_int (uint32_t number); //Funtion to print int value to serial monitor
void Serial_print_string (char string[]);

#endif  /* __SERIAL_H */