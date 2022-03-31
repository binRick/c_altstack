#ifndef CM_CRYPTO_C
#define CM_CRYPTO_C
/***********************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/***********************/
#include "./deps/c_aes/aes.c"
#include "./deps/c_aes/aes_change.c"
#include "./deps/c_aes/aes_decrypt.c"
#include "./deps/c_aes/aes_encrypt.c"
#include "./deps/c_aes/aes_key_expansion.c"
#include "./deps/c_aes/aes_padding.c"
#include "./deps/c_aes/aes_random.c"
#include "./deps/c_aes/aes_tools.c"
#include "./deps/c_aes/matrix_add_round_key.c"
#include "./deps/c_aes/matrix_mix_columns.c"
#include "./deps/c_aes/matrix_shift_rows.c"
#include "./deps/c_aes/matrix_sub_bytes.c"
/***********************/

#endif
