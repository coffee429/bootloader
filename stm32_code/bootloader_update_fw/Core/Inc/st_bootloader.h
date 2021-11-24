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

#define HEX_BUFFER_SIZE	8000


typedef struct
{
	uint8_t 	data_buffer[HEX_BUFFER_SIZE];
	uint16_t 	idx;
}Bootloader_Struct;



void bootloader_get_data(uint8_t rx_data);							// Get byte from uart, received 1 record/time -> convert to hex form & save to buffer
bool bootloader_update_firmware(uint32_t fwAdress);							// Update firmware function
void bootloader_set_address(uint32_t fwAddress);							// Set start address for bootloader

#endif /* INC_ST_BOOTLOADER_H_ */
