/**
 * @file uni_base64.h
 * @brief
 * @author nzy
 * @version 1.0.0
 * @date 2015-06-09
 */
#ifndef _UNI_BASE64_H
#define _UNI_BASE64_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief calculate the required buffer size for base64 encoding
 * @param[in] slen  source data length
 */
#define TY_BASE64_BUF_LEN_CALC(slen)   (((slen) / 3 + ((slen) % 3 != 0)) * 4 + 1)   // 1 for '\0'

/**
 * @brief tuya_base64_encode
 *
 * @param[in] bindata source data
 * @param[out] base64 base64 encoded buffer
 * @param[in] binlength source data length
 *
 * @return
 *
 * @note base64 minimum buffer size can calculated by macro TY_BASE64_BUF_LEN_CALC(binlength)
 */
char * tuya_base64_encode(const unsigned char * bindata, char * base64, int binlength);

/**
 * @brief tuya_base64_decode
 *
 * @param[out] base64
 * @param[in] bindata
 *
 * @return
 */
int tuya_base64_decode(const char * base64, unsigned char * bindata);

#ifdef __cplusplus
}
#endif
#endif

