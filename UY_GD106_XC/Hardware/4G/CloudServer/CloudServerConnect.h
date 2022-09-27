#ifndef __CLOUDSERVERCONNECT_H__
#define __CLOUDSERVERCONNECT_H__

#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>


#include "sys.h"
#include "CJSON.h"
#include "at_port.h"
#include "systick.h"
#include "flash_e2prom.h"
#include "utils_base64.h"
#include "utils_aes.h"


//@use for the cjson data praing.
typedef struct
{
    uint8_t  code;
    char 	 msg[64];
    uint8_t  key[4];
	uint32_t keybyte;
    bool	 upgrade;
	
}ServerCertification;

extern ServerCertification Certification;

/**
 * \brief         HTTP header.
 * \para[out]			calcbuffer
 */ 
void HTTP_header_encode( uint8_t output[44]);
/**
 * \brief         HTTP body.
 * \para[in]			reporttype
 * \para[in]			NumOfTransformer
 */ 
void HTTP_body(bool reporttype,uint8_t NumOfTransformer,char *cJSONout);

/**
 * \brief         save the server password.
 */
uint8_t save_password(char *rec_buff);


/**
 * \brief         dynamic registration.
 * \para[in]			body      
 * \para[out]			rec_buff
 */
bool DYNAMIC_PORT( char *rec_buff);


/**
 * \brief         send command.
 * \para[in]			cmd      
 * \para[in]	    reply1
 * \para[in]	    reply2
 * \para[out]			RXBuffer
 */
uint8_t Cmd(char *cmd,char *reply1,char *reply2, char *RXBuffer);
bool http_upload_file(char *filename);

void MQTT_password_encode(u8 *key,u8 *output);
#endif 
