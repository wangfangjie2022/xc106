#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "qcloud_connect.h"


int32_t core_sprintf(char *buffer, char *fmt, char *src[], uint8_t count)
{
    char *value = NULL;
    uint8_t idx = 0, percent_idx = 0;
    uint32_t buffer_len = 0;

    buffer_len += strlen(fmt) - 2 * count;
    for (percent_idx = 0; percent_idx < count; percent_idx++) {
        value = (*(src + percent_idx) == NULL) ? ("") : (*(src + percent_idx));
        buffer_len += strlen(value);
    }
    memset(buffer, 0, buffer_len + 1);

    for (idx = 0, percent_idx = 0; idx < strlen(fmt);) {
        if (fmt[idx] == '%' && fmt[idx + 1] == 's') {
            value = (*(src + percent_idx) == NULL) ? ("") : (*(src + percent_idx));
            memcpy(buffer + strlen(buffer), value, strlen(value));
            percent_idx++;
            idx += 2;
        } else {
            buffer[strlen(buffer)] = fmt[idx++];
        }
    }
    return 0;
}

int32_t auth_mqtt_username(char *dest, char *product_key, char *device_name)
{
    char *src[] = { device_name, product_key };

    return core_sprintf(dest, "%s&%s", src, sizeof(src)/sizeof(char *));
}

void core_hex2str(uint8_t *input, uint32_t input_len, char *output, uint8_t lowercase)
{
    char *upper = "0123456789ABCDEF";
    char *lower = "0123456789abcdef";
    char *encode = upper;
    int i = 0, j = 0;

    if (lowercase) {
        encode = lower;
    }

    for (i = 0; i < input_len; i++) {
        output[j++] = encode[(input[i] >> 4) & 0xf];
        output[j++] = encode[(input[i]) & 0xf];
    }
}

int32_t auth_mqtt_password(char *dest, char *product_key, char *device_name, char *device_secret)
{
    int32_t res = 0;
    char *src[] = { product_key, device_name, device_name, product_key };
    char *plain_text = NULL;
    uint8_t sign[32] = {0};
    plain_text = (char*)malloc(256);
    if(plain_text == NULL)
        return -1;
    res = core_sprintf(plain_text, "clientId%s.%sdeviceName%sproductKey%s", src, sizeof(src)/sizeof(char *));
    if (res < 0) {
        free(plain_text);
        return res;
    }

    memset(dest, 0, 65);

//    core_hmac_sha256((const uint8_t *)plain_text, (uint32_t)strlen(plain_text), (const uint8_t *)device_secret, (uint32_t)strlen(device_secret),sign);
    core_hex2str(sign, 32, dest, 0);
    free(plain_text);
    return 0;
}


int32_t auth_mqtt_clientid(char *dest, char *product_key, char *device_name, char *secure_mode)
{
    char *src[] = { product_key, device_name, secure_mode};

    return core_sprintf(dest, "%s.%s|securemode=%s,signmethod=hmacsha256|", src, sizeof(src)/sizeof(char *));
}

int32_t auth_mqtt_url(char* dest,char *product_key,char *region)
{
	return sprintf(dest,"%s.iotcloud.%s.com",product_key,region);
}
