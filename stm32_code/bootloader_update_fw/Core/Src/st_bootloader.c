/*
 * st_bootloader.c
 *
 *  Created on: Oct 26, 2021
 *      Author: Vo Son Tung HUST
 */
#include "st_bootloader.h"

Bootloader_Struct boot;
void bootloader_get_data(uint8_t rx_data)
{
	switch (rx_data)
	{
//		case ":":
//			break;
//		case "\n":
//			break;
		default:
			boot.data_buffer[boot.idx++] = rx_data;
			break;
	}
}





