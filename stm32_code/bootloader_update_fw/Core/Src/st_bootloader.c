/*
 * st_bootloader.c
 *
 *  Created on: Oct 26, 2021
 *      Author: Vo Son Tung HUST
 */
#include "st_bootloader.h"

Bootloader_Struct boot;
extern UART_HandleTypeDef huart2;
int byte_to_flash = 0;
int x;
void bootloader_init()
{
	boot.total_bytes = 0;
	boot.idx = 0;
	boot.flag = 0;
	memset(boot.data_buffer, 0, HEX_BUFFER_SIZE);
}
void bootloader_get_data(uint8_t rx_data)
{
	if(boot.flag == 0)
	{
		if(rx_data != '\n')
		{
			boot.total_bytes = boot.total_bytes*10 + (rx_data - '0');
		}
		else
		{
			HAL_UART_Transmit(&huart2, (uint8_t*)"1", strlen("1"), 2000);
			boot.flag = 1;
		}
	}

	else if(boot.flag == 1)
	{
		boot.data_buffer[boot.idx ++] = rx_data;
		byte_cnt ++;
		if(byte_cnt == boot.total_bytes)
		{
			boot.jump_application = 1;
		}
	}
}


void bootloader_update(uint32_t address)
{
	if(boot.jump_application == 1)
	{
		flash_erase(address, byte_to_flash);
		flash_write_array(boot.data_buffer, address, boot.total_bytes, _4_BYTE);
		boot.jump_application = 0;
		memset(boot.data_buffer, 0, HEX_BUFFER_SIZE);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

		// Deinit Pheripheral
		HAL_RCC_DeInit();
		HAL_DeInit();
		HAL_GPIO_DeInit(LED1_GPIO_Port, LED1_Pin);
		/* Turn off fault harder*/
	    SCB->SHCSR &= ~( SCB_SHCSR_USGFAULTENA_Msk |\
	    SCB_SHCSR_BUSFAULTENA_Msk | \
	    SCB_SHCSR_MEMFAULTENA_Msk ) ;

	    SysTick->CTRL = 0;
	    SysTick->LOAD = 0;
	    SysTick->VAL = 0;
	   /* Set Main Stack Pointer*/
	   __set_MSP(*((volatile uint32_t*) address));

	   uint32_t JumpAddress = *((volatile uint32_t*) (address + 4));

	   /* Set Program Counter to Blink LED Apptication Address*/
	   void (*reset_handler)(void) = (void*)JumpAddress;
	   reset_handler();

	}
}




