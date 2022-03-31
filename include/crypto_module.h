/***********************************/
#ifndef MODULE_LOG_LEVEL
#define MODULE_LOG_LEVEL    LOG_TRACE
#endif
/***********************************/
#include <stdio.h>
/***********************************/
#include "../include/module.h"
/***********************************/
/***********************************/
/***********************************/
/***********************************/
/***********************************/

#define CRYPTO_MODULE_PROPERTIES                                                                     \
  aes_key128_t key;                                                                                  \
  aes_ctx_t aes;                                                                                     \
  uint8_t   *cipher, *plain;                                                                         \
  size_t    cipher_len, plain_len;                                                                   \
  void (*setkey)(aes_key_t key);                                                                     \
  void (*setrandomkey)();                                                                            \
  char *(*getkey)();                                                                                 \
  size_t (*getkeylen)();                                                                             \
  void (*decrypt)();                                                                                 \
  uint8_t *(*encrypt)(aes_ctx_t * aes_ctx, const uint8_t *plain, size_t plain_len, size_t *enc_len); \


// `altstack_module` module definition
module(altstack_module) {
  defaults(altstack_module, CLIB_MODULE);
  void *private;

  CRYPTO_MODULE_PROPERTIES
};
///////////////////////////////////

// `altstack_module` module prototypes
static int  altstack_module_init(module(altstack_module) * exports);

static void altstack_module_deinit(module(altstack_module) * exports);

// `altstack_module` module exports
exports(altstack_module) {
  .init   = altstack_module_init,
  .deinit = altstack_module_deinit,
};

// `private` module definition
module(private) {
  define(private, CLIB_MODULE);
  CRYPTO_MODULE_PROPERTIES
};


// `private` module exports
exports(private) {
  defaults(private, CLIB_MODULE_DEFAULT),
};


// `altstack_module` module initializer
static int altstack_module_init(module(altstack_module) *exports) {
  void debug_private(){
    if (0 != exports->private) {
      log_info(AC_RESETALL AC_BOLD AC_REVERSED AC_YELLOW "init> NON PRIVATE MODE" AC_RESETALL);
    }else{
      log_info(AC_RESETALL AC_BOLD AC_REVERSED AC_BLUE "init> PRIVATE MODE" AC_RESETALL);
    }
  }

  debug_private();
  exports->private = require(private);
  debug_private();

  return(0);
}


// `altstack_module` module deinitializer
static void altstack_module_deinit(module(altstack_module) *exports) {
  log_trace("altstack_module_deinit()");
  clib_module_free((module(private) *) exports->private);
}

