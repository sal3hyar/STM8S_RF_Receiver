#include "serial.h"

///////////////////////////////////////////////////////////////// UART Functions
void Serial_print_string (char string[])
 {
	char i=0;
	while (string[i] != 0x00)
	{
		UART1_SendData8(string[i++]);
		while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
		// i++;
	}
 }

 void Serial_new_line(void)
 {
	UART1_SendData8(0x0a);
	while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET); //wait for sending
 }

 void Serial_print_int (uint32_t number) //Funtion to print int value to serial monitor
 {
	char count = 0;
	char digit[10] = "";         
	while (number != 0) //split the int to char array
	{
		digit[count++] = number%10;
		// count++;
		number /= 10;
	}


	while (count !=0) //print char array in correct direction
	{
		UART1_SendData8(digit[count-1] + 0x30);
		while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET); //wait for sending
		count--;
	}
 }
