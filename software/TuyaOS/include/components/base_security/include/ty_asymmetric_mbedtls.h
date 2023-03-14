
#ifndef __TY_ASYMMETRIC_H
#define __TY_ASYMMETRIC_H

#include "tuya_iot_config.h"
#include "tuya_tls.h"
#include "mbedtls/rsa.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/pk.h"


#define OUTPUT_MODE_NONE               0
#define OUTPUT_MODE_PRIVATE            1
#define OUTPUT_MODE_PUBLIC             2

#define OUTPUT_FORMAT_PEM              0
#define OUTPUT_FORMAT_DER              1

int tuya_gen_key(mbedtls_pk_type_t type,mbedtls_ecp_group_id grp_id, int rsa_keysize,mbedtls_pk_context *key);
int tuya_Pk_Convert_Buf( mbedtls_pk_context *key, int output_mode,int output_format,uint8_t *output_buf );
int tuya_Buf_Convert_Pk(const char *in_buf,int intput_mode,int input_format,mbedtls_pk_context *key);


int test_ty_asymmetric(void);


#endif //__TY_ASYMMETRIC_H
