#ifndef _QCLOUD_DYNREG_H_
#define _QCLOUD_DYNREG_H_
#include <stdint.h>
#include <stdbool.h>
//#include "flash_e2prom.h"


extern uint8_t Server_key[];

// typedef struct DevPart_ {			
// 	char		MQTT_ClinetId[16];			
// 	char 		MQTT_UserName[64];
// 	char		MQTT_Password[128];
// }DevPart_t;


///**
// * \brief         Get the certificate from the server.
// * \para[in]			DevPart_t   device part.
// * \return			  the state mark   /run successfully return 0, run failed return 1. 
// */
//uint8_t get_code(DevPart_t *DevPart);


/**
 * \brief         Dynamic registrate to the server.
 * \para[in]			body   		Send HTTP message body the header was be used the AT command be sent.
 * \para[in]			rec_buff	point to the receive buffer. 
 * \para[out]			none
 * \return			  the state mark   /run successfully return 0, run failed return 1. 
 */
bool DYNAMIC_PORT(char *header, char *body, char *rec_buff);


/**
 * \brief         calculate the ecb padding PKcs5|PKcs7.
 * \para[in]			data   		input data buffer body.
 * \para[in]			datalength 	the data length. 
 * \para[out]			none
 * \return			  none
 */
 #define Pkcs7 1
void calc_ecb(char *data,char datalength,unsigned char *calcbuffer);


/**
 * \brief         save the password data to flash.			
 * \para[in]			. 
 * \return			  none
 */
uint8_t save_password(void);

void HTTP_header_encode(char *data,char datalength,unsigned char *calcbuffer);

void HTTP_body(bool reporttype,uint8_t NumOfTransformer);

uint8_t qcloud_dynreg_sign( char *buff, uint16_t buff_len, char *buff_out, uint16_t len_out);

#endif