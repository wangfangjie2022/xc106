#ifndef _LV_FLASH_E2PROM_H
#define _LV_FLASH_E2PROM_H

#include "gd32f4xx.h"

//@Chip
#define GD32FLASH_SIZE    0X00020000 UL	//128KB

#define GD32FLASH_BASE		FLASH_BASE

#define GD32FLASH_AddrEND	FLASH_BASE|(GD32FLASH_SIZE-1)		

#define GD32FLASH_PAGE_SIZE (1024U)

#define GD32FLASH_PAGE_NUM	(GD332FLASH_SIZE/GD32FLASH_PAGE_SIZE)


//@The data will be change , so need to erase the flash.
#define	data_change_base_addr	(FLASH_BASE+0xFE800)	

#define addr_Factory_set_flag 	(data_change_base_addr)			//@4  bytes出厂设置标志字节

#define addr_server_key   		(data_change_base_addr+32)		//@4  bytes服务器返回字节

#define addr_url   				(data_change_base_addr+48)				//OTA下载固件地址
	
#define addr_md5				(data_change_base_addr+512)					//固件MD5值

//@The data is fixed.
#define	data_base_addr			(FLASH_BASE+0xFE000)		

#define	addr_MQTT_device_name	(data_base_addr + 20)				

#define addr_MQTT_username		(data_base_addr + 64)

#define addr_MQTT_password		(data_base_addr + 80)

uint8_t clea_e2prom(uint32_t addr);


void lv_read_e2prom(uint32_t addr,uint32_t *data);

void lv_write_e2prom(uint32_t addr,uint32_t data);

void fmc_erase_pages(uint32_t START_ADDR,uint8_t PageNum);



/* FMC sector information */
typedef struct
{
    uint32_t sector_name;                                         /*!< the name of the sector */
    uint32_t sector_num;                                          /*!< the number of the sector */
    uint32_t sector_size;                                         /*!< the size of the sector */
    uint32_t sector_start_addr;                                   /*!< the start address of the sector */
    uint32_t sector_end_addr;                                     /*!< the end address of the sector */
} fmc_sector_info_struct;

/* sector size */
#define SIZE_16KB                  ((uint32_t)0x00004000U)        /*!< size of 16KB*/
#define SIZE_64KB                  ((uint32_t)0x00010000U)        /*!< size of 64KB*/
#define SIZE_128KB                 ((uint32_t)0x00020000U)        /*!< size of 128KB*/
#define SIZE_256KB                 ((uint32_t)0x00040000U)        /*!< size of 256KB*/

/* FMC BANK address */
#define FMC_START_ADDRESS          FLASH_BASE                               /*!< FMC start address */
#define FMC_BANK0_START_ADDRESS    FMC_START_ADDRESS                        /*!< FMC BANK0 start address */
#define FMC_BANK1_START_ADDRESS    ((uint32_t)0x08100000U)                  /*!< FMC BANK1 start address */
#define FMC_SIZE                   (*(uint16_t *)0x1FFF7A22U)               /*!< FMC SIZE */
#define FMC_END_ADDRESS            (FLASH_BASE + (FMC_SIZE * 1024) - 1)     /*!< FMC end address */
#define FMC_MAX_END_ADDRESS        ((uint32_t)0x08300000U)                  /*!< FMC maximum end address */

/* FMC error message */
#define FMC_WRONG_SECTOR_NAME      ((uint32_t)0xFFFFFFFFU)        /*!< wrong sector name*/
#define FMC_WRONG_SECTOR_NUM       ((uint32_t)0xFFFFFFFFU)        /*!< wrong sector number*/
#define FMC_INVALID_SIZE           ((uint32_t)0xFFFFFFFFU)        /*!< invalid sector size*/
#define FMC_INVALID_ADDR           ((uint32_t)0xFFFFFFFFU)        /*!< invalid sector address*/

/* get the sector number, size and range of the given address */
fmc_sector_info_struct fmc_sector_info_get(uint32_t addr);
/* get the sector number by sector name */
uint32_t sector_name_to_number(uint32_t sector_name);
/* erases the sector of a given sector number */
void fmc_erase_sector_by_address(uint32_t address);
/* write 32 bit length data to a given address */
void fmc_write_32bit_data(uint32_t address, uint16_t length, uint32_t* data_32);
/* read 32 bit length data from a given address */
void fmc_read_32bit_data(uint32_t address, uint16_t length, int32_t* data_32);
/* write 16 bit length data to a given address */
void fmc_write_16bit_data(uint32_t address, uint16_t length, int16_t* data_16);
/* read 16 bit length data from a given address */
void fmc_read_16bit_data(uint32_t address, uint16_t length, int16_t* data_16);
/* write 8 bit length data to a given address */
void fmc_write_8bit_data(uint32_t address, uint16_t length, int8_t* data_8);
/* read 8 bit length data from a given address */
void fmc_read_8bit_data(uint32_t address, uint16_t length, int8_t* data_8);

void FLASH_init(void);

#endif 

