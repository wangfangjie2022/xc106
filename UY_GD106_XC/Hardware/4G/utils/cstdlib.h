#ifndef __CSTDLIB_H__
#define __CSTDLIB_H__

#include "stdio.h"
#include <stdint.h>
#include "stdbool.h"
char* itoa(int num,char* str,int radix);
char* AsciiHexNumber(uint8_t num,char* str);
void Bitmerge(bool mode,uint8_t *source_data,uint32_t *data);
#endif 
