#ifndef __AT_PORT_H
#define __AT_PORT_H	 

#include "cat1_port.h"
#include "at_parser.h"
#include "at_module.h"


void ATModuleInit(pATGetSystick f);

extern ATModuleConfig      g_ATModule;
void MakeMqttParserConfigCMD(const char* clientid,const char* username,const char* password,char buff[],int buffLen);

#endif

