#include "main.h"
#include "serial.h"

#define frame_size 48
static volatile uint16_t rf_data_length[frame_size];
static volatile uint8_t rf_data_index = 0;

static volatile uint16_t syncbit_width = 0;

static volatile uint32_t ev1527_data = 0;
static volatile bool rf_data_ready = FALSE;
static volatile bool sync_detected = FALSE;
static volatile uint8_t last_edge = 0;
static volatile uint8_t current_edge = 0;

static volatile uint16_t timer_value = 0;

/////////////////////////////////////////////////////////////////// Setup Functions
void EXTI2_Setup(void) {
	EXTI_DeInit();
	EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_RISE_FALL);
}

void GPIO_Setup(void)
{
	GPIO_DeInit(EV1527_PORT);
	GPIO_Init(EV1527_PORT,EV1527_PIN,GPIO_MODE_IN_PU_IT);

	GPIO_DeInit(GPIOD);
	GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
}

void UART1_Setup(void) {
	UART1_DeInit();
	UART1_Init(250000, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TX_ENABLE);
}

void TIM2_Setup(void) {
	TIM2_DeInit();
	TIM2_TimeBaseInit(TIM2_PRESCALER_32, 0xFFFF); // 2MHz / 32 = 62.5 KHz
	TIM2_Cmd(ENABLE);
}

void Clock_Setup(void)
{
      CLK_DeInit();
                
      CLK_HSECmd(DISABLE);
      CLK_LSICmd(DISABLE);
      CLK_HSICmd(ENABLE);
      while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
                
      CLK_ClockSwitchCmd(ENABLE);
	 
      CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV2); // 16MHz / 2 = 8MHz
      CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
                
      CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, 
      DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);


	  // enable desired peripherals
	  CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
	  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
	  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
	  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
	  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);
	  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
	  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
	  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);            
}

////////////////////////////////////////////////////////////////// Interrupt
INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
{
	timer_value = TIM2_GetCounter();
	TIM2_SetCounter(0);
	current_edge = GPIO_ReadInputPin(EV1527_PORT, EV1527_PIN);

	// Noise detection
   	if (timer_value < Noise_threshold || last_edge == current_edge || timer_value > Sync_Max_width)
	{
		last_edge = current_edge;
		sync_detected = 0;
		return;
	}
	else
	{
		last_edge = current_edge;
	}


	if (!sync_detected) 
	{
		// Check for sync bit (longer pulse) 
		if ((current_edge) && (timer_value > Sync_Min_width) && (timer_value < Sync_Max_width)) {
			sync_detected = TRUE;
			rf_data_index = 0;
			syncbit_width = timer_value;
		}
	} 
 	else 
	{
		rf_data_length[rf_data_index++] = timer_value;


		if(rf_data_index == frame_size)
		{
			rf_data_ready = TRUE;
		}
	} 

}

////////////////////////////////////////////////////////////////// Functions
/*  
uint8_t Check_RF_Data(void)
 {
	uint16_t pulse_0_min = (syncbit_width / 31) - 20;
	uint16_t pulse_0_max = (syncbit_width / 31) + 20;
	uint16_t pulse_1_min = ((syncbit_width / 31) * 3) - 20;
	uint16_t pulse_1_max = ((syncbit_width / 31) * 3) + 20;
	uint8_t bit_index = 0;

	for (bit_index = 0; bit_index < frame_size; bit_index++)
	{
		if((rf_data_length[bit_index] > pulse_1_min) && (rf_data_length[bit_index] < pulse_1_max))
		{
			continue;	
		}
		else if ((rf_data_length[bit_index] > pulse_0_min) && (rf_data_length[bit_index] < pulse_0_max))
		{
			continue;
		}
		else
			return 0;
	}
	return 1;	
}
 */
void Ev1527_Parse_data(void)
{
	Serial_print_string("Sync-Bit Length = ");
	Serial_print_int((uint32_t)syncbit_width);
	Serial_print_string("(ticks)");
	Serial_new_line();


	Serial_print_string("Received Data Frame = ");

	uint8_t bit_index=0;
	for (bit_index = 0; bit_index < frame_size; bit_index+=2)
	{
		if(abs(rf_data_length[bit_index] - rf_data_length[bit_index+1]) < Noise_threshold)
		{
			Serial_print_string("Noise Detected!!!\n\n");
			return;
		}

		if (rf_data_length[bit_index] > rf_data_length[bit_index+1])
		{
			UART1_SendData8('1');
			ev1527_data = (ev1527_data << 1) | 1;
		}
		else
		{
			UART1_SendData8('0');
			ev1527_data = (ev1527_data << 1);
		}
		while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET); //wait for sending
	}
	Serial_new_line();
	Serial_print_string("Remote Code = ");
	Serial_print_int(ev1527_data >> 4);

	Serial_new_line();
	Serial_print_string("Pressed Key Name = ");

	uint8_t rf_key= ev1527_data & 0x0000000f;

	switch (rf_key) {
		case A_key:
			Serial_print_string("A\n");
			break;
		case B_key:
			Serial_print_string("B\n");
			break;
		case C_key:
			Serial_print_string("C\n");
			break;
		case D_key:
			Serial_print_string("D\n");
			break;
		case AB_key:
			Serial_print_string("A+B\n");
			break;
		case AC_key:
			Serial_print_string("A+C\n");
			break;
		case AD_key:
			Serial_print_string("A+D\n");
			break;
		case BC_key:
			Serial_print_string("B+C\n");
			break;
		case BD_Key:
			Serial_print_string("B+D\n");
			break;
		case CD_key:
			Serial_print_string("C+D\n");
			break;
		default:
			Serial_print_string("N/A\n");
			break;
	}

	Serial_print_string("Pressed Key Code = ");
	Serial_print_int((uint32_t)rf_key);
	Serial_new_line();
	Serial_print_string("---------- End----------");
	Serial_new_line();
}

////////////////////////////////////////////////////////////////// Main Loop
void main(void) {
	Clock_Setup();
	GPIO_Setup();
	UART1_Setup();
	EXTI2_Setup();
	TIM2_Setup();

	_delay_ms(100);
	uint8_t ii=0;
	for (ii = 0; ii < 6; ii++)
	{
		GPIO_WriteReverse(LED_PORT, LED_PIN);
		_delay_ms(100);
	}
	
	Serial_print_string("MCU is Ready.\n");
	enableInterrupts();

	while (1) {

 		if(rf_data_ready)
		{
/* 
			if(Check_RF_Data()){
				Serial_print_string("Good data received.\n");
			}
			else
			{
				Serial_print_string("Bad data received.\n");
			} 
*/
			
			Ev1527_Parse_data();
			
			GPIO_WriteReverse(LED_PORT,LED_PIN);

			// delay before get new data
			_delay_ms(500);

			// prepare to get new data
			ev1527_data = 0;
			sync_detected = FALSE;
			rf_data_ready = FALSE;

		} 
	}
}