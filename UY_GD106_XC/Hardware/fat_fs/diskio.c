/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "sdcard.h"
#include <stdio.h>

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */
/*-----------------------------------------------------------------------*/
#define BLOCKSIZE   512      /* block size in bytes */                                              
//#define BUSMODE_4BIT         /* SD 4-bit bus mode, uncommend this macro to choose 1-bit bus mode */
#define DMA_MODE             /* SD DMA mode, uncommend this macro to choose polling mode */
sd_error_enum status;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE drv                /* Physical drive nmuber (0..) */
)
{
    sd_card_info_struct sd_cardinfo;
    uint32_t cardstate = 0;
    if(0 == drv){
        /* initialize the card */
        status = sd_init();
        if(SD_OK == status){
            status = sd_card_information_get(&sd_cardinfo);
        }else{
            return STA_NOINIT;
        }
        if(SD_OK == status){
            status = sd_card_select_deselect(sd_cardinfo.card_rca);
        }else{
            return STA_NOINIT;
        }
        status = sd_cardstatus_get(&cardstate);
        if(cardstate & 0x02000000){
            /* the card is locked */
            while (1){
            }
        }
        /* configure the bus mode and data transfer mode */
        if(SD_OK == status){
            /* set bus mode */
#ifdef BUSMODE_4BIT
            status = sd_bus_mode_config(SDIO_BUSMODE_4BIT);
#else
            status = sd_bus_mode_config( SDIO_BUSMODE_1BIT );
#endif /* BUSMODE_4BIT */
        }else{
            return STA_NOINIT;
        }
        if(SD_OK == status){
            /* set data transfer mode */
#ifdef DMA_MODE
            status = sd_transfer_mode_config( SD_DMA_MODE );
#else
            status = sd_transfer_mode_config( SD_POLLING_MODE );
#endif /* DMA_MODE */
        }else{
            return STA_NOINIT;
        }
        if(SD_OK == status){
            /* initialize success */
            return 0;
        }else{
            return STA_NOINIT;
        }
    }else{
        /* only support physical drive nmuber 0 */
        return STA_NOINIT;
    }
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE drv         /* Physical drive nmuber (0..) */
)
{
    if(0 == drv){
        return RES_OK;
    }


    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE drv,          /* Physical drive nmuber (0..) */
    BYTE *buff,        /* Data buffer to store read data */
    DWORD sector,      /* Sector address (LBA) */
    BYTE count         /* Number of sectors to read (1..255) */
)
{
    sd_error_enum status = SD_ERROR;
    /* check the correctness of the parameters */
    if(NULL == buff){
        return RES_PARERR;
    }
    if(!count){
        return RES_PARERR;
    }

    if(0 == drv){
        if(1 == count){
            /* single sector read */
            status = sd_block_read((uint32_t *)(&buff[0]), (uint32_t)(sector<<9), BLOCKSIZE);
        }else{
            /* multiple sectors read */
            status = sd_multiblocks_read((uint32_t *)(&buff[0]), (uint32_t)(sector<<9), BLOCKSIZE, (uint32_t)count);
        }
    }
    if(SD_OK == status){
        return RES_OK;
    }
    return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
/* The FatFs module will issue multiple sector transfer request
/  (count > 1) to the disk I/O layer. The disk function should process
/  the multiple sector transfer properly Do. not translate it into
/  multiple single sector transfers to the media, or the data read/write
/  performance may be drasticaly decreased. */

#if _READONLY == 0
DRESULT disk_write (
    BYTE drv,            /* Physical drive nmuber (0..) */
    const BYTE *buff,    /* Data to be written */
    DWORD sector,        /* Sector address (LBA) */
    BYTE count           /* Number of sectors to write (1..255) */
)
{
    sd_error_enum status = SD_ERROR;
    /* check the correctness of the parameters */
    if(NULL == buff){
        return RES_PARERR;
    }
    if(!count){
        return RES_PARERR;
    }

    if(0 == drv){
        if(1 == count){
            /* single sector write */
            status = sd_block_write((uint32_t *)buff, sector<<9, BLOCKSIZE);
        }else{
            /* multiple sectors write */
            status = sd_multiblocks_write((uint32_t *)buff, sector<<9, BLOCKSIZE, (uint32_t)count);
        }
    }
    if(SD_OK == status){
        return RES_OK;
    }
    return RES_ERROR;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE drv,         /* Physical drive nmuber (0..) */
    BYTE ctrl,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    
    return RES_OK;
    
}
 
/*-----------------------------------------------------------------------*/
/* Get current time                                                      */
/*-----------------------------------------------------------------------*/ 
DWORD get_fattime(void)
{

  return 0;

}
