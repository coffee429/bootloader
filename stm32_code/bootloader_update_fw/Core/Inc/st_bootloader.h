/*
 * st_bootloader.h
 *
 *  Created on: Oct 26, 2021
 *      Author: Vo Son Tung HUST
 */

#ifndef INC_ST_BOOTLOADER_H_
#define INC_ST_BOOTLOADER_H_

#include "main.h"
#include "st_flash.h"
#include <string.h>

#define HEX_BUFFER_SIZE	6000
uint16_t byte_cnt;
typedef struct
{
	uint8_t 	data_buffer[HEX_BUFFER_SIZE];
	int 		total_bytes;
	uint16_t 	idx;
	uint8_t 	flag;
	uint8_t		jump_application;
}Bootloader_Struct;

void bootloader_init();
void bootloader_get_data(uint8_t rx_data);									// Get byte from uart, received 1 record/time -> convert to hex form & save to buffer
void bootloader_update(uint32_t address);
#endif /* INC_ST_BOOTLOADER_H_ */
