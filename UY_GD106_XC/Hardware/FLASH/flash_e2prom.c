#include "header.h"

uint32_t wp_value = 0xFFFFFFFF;

#include "gd32f4xx.h"


//读FLASH
void lv_read_e2prom(uint32_t addr,uint32_t *data)
{
		*data =*(uint32_t*)addr; 
		
}

uint8_t clea_e2prom(uint32_t addr)
{
	addr |= 0x3FF;
	addr -= 0x3FF;
	uint32_t addrLast = addr+0x3FF;
	while(addr<addrLast)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
	{
		if(*(uint32_t *)addr != 0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
		{
			fmc_erase_sector_by_address(addr);	//擦除页
		}
		else
			addr += 4;
	}
	return 0;
}


void lv_write_e2prom(uint32_t addr,uint32_t data)
{
	fmc_write_32bit_data(addr, 1, &data);
}


/*!
    \brief      erase fmc pages from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_pages(uint32_t START_ADDR,uint8_t PageNum)
{
    uint32_t EraseCounter;
    /* erase the flash pages */
    for(EraseCounter = 0; EraseCounter < PageNum; EraseCounter++)
		{
        fmc_erase_sector_by_address(START_ADDR + ((uint16_t)0x400U * EraseCounter));
    }

}



//AHB 120MHz
void FLASH_init(void)
{
	
//	fmc_wscnt_set(FMC_WAIT_STATE_3);
//	fmc_ibus_disable();
//	fmc_dbus_disable();
//	fmc_ibus_reset();
//	fmc_dbus_reset();
//	fmc_unlock();
//	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR);
//	fmc_erase_pages(data_base_addr,1);
//	fmc_erase_pages(data_change_base_addr,1);
//	fmc_program_width_set(FMC_PROG_W_32B);

//	fmc_lock();
}


/*!
    \brief      get the sector number, size and range of the given address
    \param[in]  address: The flash address
    \param[out] none
    \retval     fmc_sector_info_struct: The information of a sector
*/
fmc_sector_info_struct fmc_sector_info_get(uint32_t addr)
{
    fmc_sector_info_struct sector_info;
    uint32_t temp = 0x00000000U;
    if((FMC_START_ADDRESS <= addr)&&(FMC_END_ADDRESS >= addr)) {
        if ((FMC_BANK1_START_ADDRESS > addr)) {
            /* bank0 area */
            temp = (addr - FMC_BANK0_START_ADDRESS) / SIZE_16KB;
            if (4U > temp) {
                sector_info.sector_name = (uint32_t)temp;
                sector_info.sector_num = CTL_SN(temp);
                sector_info.sector_size = SIZE_16KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_16KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_16KB - 1;
            } else if (8U > temp) {
                sector_info.sector_name = 0x00000004U;
                sector_info.sector_num = CTL_SN(4);
                sector_info.sector_size = SIZE_64KB;
                sector_info.sector_start_addr = 0x08010000U;
                sector_info.sector_end_addr = 0x0801FFFFU;
            } else {
                temp = (addr - FMC_BANK0_START_ADDRESS) / SIZE_128KB;
                sector_info.sector_name = (uint32_t)(temp + 4);
                sector_info.sector_num = CTL_SN(temp + 4);
                sector_info.sector_size = SIZE_128KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_128KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_128KB - 1;
            }
        } else {
            /* bank1 area */
            temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_16KB;
            if (4U > temp) {
                sector_info.sector_name = (uint32_t)(temp + 12);
                sector_info.sector_num = CTL_SN(temp + 16);
                sector_info.sector_size = SIZE_16KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_16KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_16KB - 1;
            } else if (8U > temp) {
                sector_info.sector_name = 0x00000010;
                sector_info.sector_num = CTL_SN(20);
                sector_info.sector_size = SIZE_64KB;
                sector_info.sector_start_addr = 0x08110000U;
                sector_info.sector_end_addr = 0x0811FFFFU;
            } else if (64U > temp){
                temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_128KB;
                sector_info.sector_name = (uint32_t)(temp + 16);
                sector_info.sector_num = CTL_SN(temp + 20);
                sector_info.sector_size = SIZE_128KB;
                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_128KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_128KB - 1;
            } else {
                temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_256KB;
                sector_info.sector_name = (uint32_t)(temp + 20);
                sector_info.sector_num = CTL_SN(temp + 8);
                sector_info.sector_size = SIZE_256KB;
                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_256KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_256KB - 1;
            }
        }
    } else {
        /* invalid address */
        sector_info.sector_name = FMC_WRONG_SECTOR_NAME;
        sector_info.sector_num = FMC_WRONG_SECTOR_NUM;
        sector_info.sector_size = FMC_INVALID_SIZE;
        sector_info.sector_start_addr = FMC_INVALID_ADDR;
        sector_info.sector_end_addr = FMC_INVALID_ADDR;
    }
    return sector_info;
}

/*!
    \brief      get the sector number by a given sector name
    \param[in]  address: a given sector name
    \param[out] none
    \retval     uint32_t: sector number
*/
uint32_t sector_name_to_number(uint32_t sector_name)
{
    if(11 >= sector_name){
        return CTL_SN(sector_name);
    }else if(23 >= sector_name){
        return CTL_SN(sector_name + 4);
    }else if(27 >= sector_name){
        return CTL_SN(sector_name - 12);
    }else{
        while(1);
    }
}

/*!
    \brief      erases the sector of a given address
    \param[in]  address: a given address
    \param[out] none
    \retval     none
*/
void fmc_erase_sector_by_address(uint32_t address)
{
    fmc_sector_info_struct sector_info;

    /* get information about the sector in which the specified address is located */
    sector_info = fmc_sector_info_get(address);
    if(FMC_WRONG_SECTOR_NAME == sector_info.sector_name)
		{

        while(1);
    }
		else
		{

        /* unlock the flash program erase controller */
        fmc_unlock(); 
        /* clear pending flags */
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
        /* wait the erase operation complete*/
        if(FMC_READY != fmc_sector_erase(sector_info.sector_num)){
            while(1);
        }
        /* lock the flash program erase controller */
        fmc_lock();

    }
}

/*!
    \brief      write 32 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_32: data pointer
    \param[out] none
    \retval     none
*/
void fmc_write_32bit_data(uint32_t address, uint16_t length, uint32_t* data_32)
{
//    fmc_sector_info_struct start_sector_info;
//    fmc_sector_info_struct end_sector_info;
//    uint32_t sector_num;
    uint32_t i;
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* get the information of the start and end sectors */
//    start_sector_info = fmc_sector_info_get(address);
//    end_sector_info = fmc_sector_info_get(address + 4*length);
//    /* erase sector */
//    for(i = start_sector_info.sector_name; i <= end_sector_info.sector_name; i++){
//        sector_num = sector_name_to_number(i);
//        if(FMC_READY != fmc_sector_erase(sector_num)){
//            while(1);
//        }
//    }

    /* write data_32 to the corresponding address */
    for(i=0; i<length; i++){
        if(FMC_READY == fmc_word_program(address, data_32[i])){
            address = address + 4;
        }else{ 
            while(1);
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();

}

/*!
    \brief      read 32 bit length data from a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_32: data pointer
    \param[out] none
    \retval     none
*/
void fmc_read_32bit_data(uint32_t address, uint16_t length, int32_t* data_32)
{
    uint8_t i;

    for(i=0; i<length; i++){
        data_32[i] = *(__IO int32_t*)address;

        address=address + 4;
    }
}

/*!
    \brief      write 16 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_16: data pointer
    \param[out] none
    \retval     none
*/
void fmc_write_16bit_data(uint32_t address, uint16_t length, int16_t* data_16)
{
    fmc_sector_info_struct start_sector_info;
    fmc_sector_info_struct end_sector_info;
    uint32_t sector_num,i;
    
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* get the information of the start and end sectors */
    start_sector_info = fmc_sector_info_get(address);
    end_sector_info = fmc_sector_info_get(address + 2*length);
    /* erase sector */
    for(i = start_sector_info.sector_name; i <= end_sector_info.sector_name; i++){
        sector_num = sector_name_to_number(i);
        if(FMC_READY != fmc_sector_erase(sector_num)){
            while(1);
        }
    }

    /* write data_16 to the corresponding address */
    for(i=0; i<length; i++){
        if(FMC_READY == fmc_halfword_program(address, data_16[i])){
            address = address + 2;
        }else{ 
            while(1);
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();

}

/*!
    \brief      read 16 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_16: data pointer
    \param[out] none
    \retval     none
*/
void fmc_read_16bit_data(uint32_t address, uint16_t length, int16_t* data_16)
{
    uint8_t i;

    for(i=0; i<length; i++){
        data_16[i] = *(__IO int16_t*)address;

        address = address + 2;
    }

}

/*!
    \brief      write 8 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_8: data pointer
    \param[out] none
    \retval     none
*/
void fmc_write_8bit_data(uint32_t address, uint16_t length, int8_t* data_8)
{
    fmc_sector_info_struct start_sector_info;
    fmc_sector_info_struct end_sector_info;
    uint32_t sector_num,i;
    
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* get the information of the start and end sectors */
    start_sector_info = fmc_sector_info_get(address);
    end_sector_info = fmc_sector_info_get(address + 2*length);
    /* erase sector */
    for(i = start_sector_info.sector_name; i <= end_sector_info.sector_name; i++){
        sector_num = sector_name_to_number(i);
        if(FMC_READY != fmc_sector_erase(sector_num)){
            while(1);
        }
    }

    /* write data_8 to the corresponding address */
    for(i=0; i<length; i++){
        if(FMC_READY == fmc_byte_program(address, data_8[i])){
            address++;
        }else{
            while(1);
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();
}

/*!
    \brief      read 8 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_8: data pointer
    \param[out] none
    \retval     none
*/
void fmc_read_8bit_data(uint32_t address, uint16_t length, int8_t* data_8)
{
    uint8_t i;

    for(i=0; i<length; i++)
		{
        data_8[i] = *(__IO int8_t*)address;

        address++;
    }

}

