/*
 * st_flash.h
 *
 *  Created on: Oct 23, 2021
 *      Author: Vo Son Tung HUST
 */

#ifndef INC_ST_FLASH_H_
#define INC_ST_FLASH_H_

#include "main.h"
#include  <stdio.h>
#include <stdbool.h>


typedef enum
{
	BYTE_1,
	BYTE_2,
	BYTE_4,
}DataTypedef;

uint32_t flash_get_sector(uint32_t address);
void flash_erase(uint32_t startAddress, uint8_t length);

void flash_write_array(void *bufferW, uint32_t startAddress, uint32_t size, DataTypedef DATA_TYPE);
void flash_write_int(uint32_t address, int data);
void flash_write_float(uint32_t address, float f);


void flash_read_array(void *bufferR, uint32_t startAddress, uint32_t size, DataTypedef DATA_TYPE);
int flash_read_int(uint32_t address);
float flash_read_float(uint32_t address);

#endif /* INC_ST_FLASH_H_ */
