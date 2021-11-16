/*
 * st_bootloader.c
 *
 *  Created on: Oct 26, 2021
 *      Author: Vo Son Tung HUST
 */
#include "st_bootloader.h"

Hex_File_Struct hexFile;


bool check_hex_file()
{
	if((*hexFileBuffer == 0x02) && (*(hexFileBuffer + 1) == 0x00) && (*(hexFileBuffer + 2) == 0x00) && (*(hexFileBuffer + 3) == 0x04))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool check_new_firmware()
{
	return newFw;
}


uint8_t convert_str_to_hex(uint8_t highByte, uint8_t lowByte)
{
	if(highByte >= '0' && highByte <= '9')			highByte = highByte - '0';
	else if(highByte >= 'A' && highByte <= 'F')		highByte = highByte - 'A' + 10;

	if(lowByte >= '0' && lowByte <= '9')			lowByte = lowByte - '0';
	else if(lowByte >= 'A' && lowByte <= 'F')		lowByte = lowByte - 'A' + 10;

	return (highByte << 4 | lowByte);
}


void swap_byte(uint8_t* swapBuffer, uint8_t* dataBuffer, uint8_t pos)
{
	uint8_t temp;

	// Swap byte 1 and byte 4
	temp = *(swapBuffer + pos);
	*(swapBuffer + pos) = *(swapBuffer + pos + 3);
	*(swapBuffer + pos + 3) = temp;

	// Swap byte 2 and byte 3
	temp = *(swapBuffer + pos + 1);
	*(swapBuffer + pos + 1) = *(swapBuffer + pos + 2);
	*(swapBuffer + pos + 2) = temp;

	// Save to buffer for writing flash
	for(int i=0; i<4; i++)
	{
		*(dataBuffer + i) = *(swapBuffer + pos + i);
	}
}


void bootloader_uart_get_hex_file(uint8_t rxData)
{
	static uint8_t byteCnt;																						// Counting byte variable, 2 character in a row -> 1 hex byte
																												// '0' 'A' -> '0A' (0x0A)
	switch (rxData)
	{
		case ':':																								// Start record character
			byteCnt = 0;																						//
			recordNum ++;
			break;
		case '\n':																								// End record character
			hexFileBuffer[hexFile.Index ++] = '\n';
			break;
		default:																								// Other character
			if(byteCnt == 0)
			{
				hexFile.HighByte = rxData;																		// High byte = first byte
				byteCnt ++;
			}
			else if(byteCnt == 1)
			{
				hexFile.LowByte  = rxData;																		// Low byte = second byte
				hexFileBuffer[hexFile.Index ++] = convert_str_to_hex(hexFile.HighByte, hexFile.LowByte);
				byteCnt = 0;
			}
			break;
	}
}



void bootloader_set_address(uint32_t fwAddress)
{
	newFwAddress = fwAddress;
}

bool bootloader_update_firmware(uint32_t fwAddress)
{
	if(!check_hex_file())
	{
		return false;
	}
	bootloader_set_address(fwAddress);
	/* 10 C000 00 0000022025C70008A5C60008ABC60008 2E */
	char* record = strtok((char*)hexFileBuffer, "\n");

	while(record != NULL)
	{
		record = strtok(NULL, "\n");
		switch (*record)						    // Count number of data in data record
		{
			case 0x04:								// 4 byte data
				hexFile.DataCount = 4;
				break;
			case 0x08:								// 8 byte data
				hexFile.DataCount = 8;
				break;
			case 0x0C:								// 12 byte data
				hexFile.DataCount = 12;
				break;
			case 0x10:								// 16 byte data
				hexFile.DataCount = 16;
				break;
		}

		if(*(record + 3) == 0x00)					// Check record type 0x00 -> data record
		{
			int cnt = 0;
			while(cnt < hexFile.DataCount/4)
			{
				swap_byte(hexFileBuffer, flashWriteBuffer, 4 + cnt*4);
				cnt ++;
				flash_write_array(flashWriteBuffer, newFwAddress, 4, FLASH_TYPEPROGRAM_WORD);
				newFwAddress += 4;
			}
		}
	}
	memset(hexFileBuffer, 0, HEX_BUFFER_SIZE);
	return true;
}






