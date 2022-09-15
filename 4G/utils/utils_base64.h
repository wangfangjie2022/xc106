/*
 * Tencent is pleased to support the open source community by making IoT Hub
 available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file
 except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software
 distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND,
 * either express or implied. See the License for the specific language
 governing permissions and
 * limitations under the License.
 *
 */

#ifndef QCLOUD_IOT_UTILS_BASE64_H_
#define QCLOUD_IOT_UTILS_BASE64_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>



/**
 * \brief          Encode a buffer into base64 format
 *
 * \param dst      destination buffer
 * \param dlen     size of the destination buffer
 * \param olen     number of bytes written
 * \param src      source buffer
 * \param slen     amount of data to be encoded
 *
 * \return         0 if successful, or MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL.
 *                 *olen is always updated to reflect the amount
 *                 of data that has (or would have) been written.
 *                 If that length cannot be represented, then no data is
 *                 written to the buffer and *olen is set to the maximum
 *                 length representable as a size_t.
 *
 * \note           Call this function with dlen = 0 to obtain the
 *                 required buffer size in *olen
 */
int qcloud_iot_utils_base64encode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen);


/**
 * \brief          Decode a base64-formatted buffer
 *
 * \param dst      destination buffer (can be NULL for checking size)
 * \param dlen     size of the destination buffer
 * \param olen     number of bytes written
 * \param src      source buffer
 * \param slen     amount of data to be decoded
 *
 * \return         0 if successful, MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL, or
 *                 MBEDTLS_ERR_BASE64_INVALID_CHARACTER if the input data is
 *                 not correct. *olen is always updated to reflect the amount
 *                 of data that has (or would have) been written.
 *
 * \note           Call this function with *dst = NULL or dlen = 0 to obtain
 *                 the required buffer size in *olen
 */
int qcloud_iot_utils_base64decode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen);

#ifdef __cplusplus
}
#endif
#endif /* QCLOUD_IOT_UTILS_BASE64_H_ */
