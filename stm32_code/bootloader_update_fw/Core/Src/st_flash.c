/*
 * st_flash.c
 *
 *  Created on: Oct 23, 2021
 *      Author: Vo Son Tung HUST
 */


#include "st_flash.h"

uint32_t flash_get_sector(uint32_t address)
{
	uint32_t sector = 0;

  /* BANK 1 */
  if((address >= 0x08000000) && (address < 0x08004000))
  {
	sector = FLASH_SECTOR_0;
  }

  else if((address >= 0x08004000) && (address < 0x08008000))
  {
	sector = FLASH_SECTOR_1;
  }

  else if((address >= 0x08008000) && (address < 0x0800C000))
  {
	sector = FLASH_SECTOR_2;
  }

  else if((address >= 0x0800C000) && (address < 0x08010000))
  {
	sector = FLASH_SECTOR_3;
  }

  else if((address >= 0x08010000) && (address < 0x08020000))
  {
	sector = FLASH_SECTOR_4;
  }

  else if((address >= 0x08020000) && (address < 0x08040000))
  {
	sector = FLASH_SECTOR_5;
  }

  else if((address >= 0x08040000) && (address < 0x08060000))
  {
	sector = FLASH_SECTOR_6;
  }

  else if((address >= 0x08060000) && (address < 0x0807FFFF))
  {
	sector = FLASH_SECTOR_7;
  }
  return sector;
}



void flash_erase(uint32_t startAddress, uint8_t size)
{
	uint32_t startSector = flash_get_sector(startAddress);
	uint32_t endAddress  = startAddress + size*4;
	uint32_t endSector   = flash_get_sector(endAddress);

	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.Banks = 1;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.Sector = flash_get_sector(startAddress);
	EraseInitStruct.NbSectors = endSector - startSector + 1;

	uint32_t error;
	HAL_FLASHEx_Erase(&EraseInitStruct, &error);
	HAL_FLASH_Lock();
}

void flash_write_int(uint32_t address, int data)
{
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, data);
	HAL_FLASH_Lock();

}

void flash_write_float(uint32_t address, float f)
{
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *(uint32_t*)&f);
	HAL_FLASH_Lock();
}

void flash_write_array(void *bufferW, uint32_t startAddress, uint32_t size, DataTypedef DATA_TYPE)
{
	uint32_t wAddress = startAddress;
	HAL_FLASH_Unlock();

	switch(DATA_TYPE)
	{
		case _1_BYTE:
			for(uint8_t i=0; i< size; i++)
			{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, wAddress, ((uint8_t*)bufferW)[i]);
				wAddress ++;
			}
			break;

		case _2_BYTE:
			for(uint16_t i=0; i< size; i++)
			{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, wAddress, ((uint16_t*)bufferW)[i]);
				wAddress += 2;
			}
			break;

		case _4_BYTE:
			for(uint32_t i=0; i< size; i++)
			{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, wAddress, ((uint32_t*)bufferW)[i]);
				wAddress += 4;
			}
			break;
	}
	HAL_FLASH_Lock();
}




int flash_read_int(uint32_t address)
{
	return *(__IO uint16_t*)(address);
}

float flash_read_float(uint32_t address)
{
	uint32_t data = *(__IO uint32_t*)(address);
	return *(float*)(&data);
}

void flash_read_array(void *bufferR, uint32_t startAddress, uint32_t size, DataTypedef DATA_TYPE)
{
	uint32_t rAddress = startAddress;
	switch(DATA_TYPE)
	{
		case _1_BYTE:
			for(uint8_t i=0; i<size; i++)
			{
				*((uint8_t*)bufferR + i) = *(uint8_t*)rAddress;
				rAddress ++;
			}
			break;
		case _2_BYTE:
			for(uint16_t i=0; i<size; i++)
			{
				*((uint16_t*)bufferR + i) = *(uint16_t*)rAddress;
				rAddress += 2;
			}
			break;
		case _4_BYTE:
			for(uint32_t i=0; i<size; i++)
			{
				*((uint32_t*)bufferR + i) = *(uint32_t*)rAddress;
				rAddress += 4;
			}
			break;
	}
}
