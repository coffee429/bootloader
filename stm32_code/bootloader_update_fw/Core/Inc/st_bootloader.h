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
uint8_t hexFileBuffer[HEX_BUFFER_SIZE];				// Hex file recived in string form will convert to hex form and save to this buffer
uint8_t flashWriteBuffer[4];						// Hex data in each record(line) in hex file that need to write to flash will save in this buffer
uint16_t recordNum;									// Number of record (line) in hex file
uint32_t newFwAddress;								// Address to write new firmware
bool newFw;

typedef struct										// Hex file struct
{
	uint16_t Index;									// Index of byte received from uart
	uint8_t HighByte;								// High byte to convert string to hex
	uint8_t LowByte;								// Low byte to convert string to hex
	uint8_t DataCount;							// Number of data byte in each record
}Hex_File_Struct;



void bootloader_uart_get_hex_file(uint8_t rxData);										// Get byte from uart, received 1 record/time -> convert to hex form & save to buffer
bool bootloader_update_firmware(uint32_t fwAdress);								// Update firmware function
void bootloader_set_address(uint32_t fwAddress);										// Set start address for bootloader

bool check_new_firmware();
bool check_hex_file();												// Hex file checking function
void swap_byte(uint8_t* swapBuffer, uint8_t* dataBuffer, uint8_t position);				// Swap byte to write to flash
uint8_t convert_str_to_hex(uint8_t highByte, uint8_t lowByte);							// Convert strig to hex function
#endif /* INC_ST_BOOTLOADER_H_ */
