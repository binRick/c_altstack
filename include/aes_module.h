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
const char *DEFAULT_KEY = "0123456789abcdef";
/***********************************/

/****************************************************************************************************/
#define PRIVATE_AES_MODULE_PROPERTIES \
  void *private;                      \
  aes_key128_t key;                   \
  char         *k;                    \
  aes_ctx_t    aes;                   \
  uint8_t      *cipher, *plain;       \
  size_t       cipher_len, plain_len; \
/****************************************************************************************************/
#define PUBLIC_AES_MODULE_PROPERTIES                                                                 \
  bool (*setkey)(char *);                                                                            \
  void (*setrandomkey)();                                                                            \
  char *(*getkey)();                                                                                 \
  size_t (*getkeylen)();                                                                             \
  void (*decrypt)();                                                                                 \
  uint8_t *(*encrypt)(aes_ctx_t * aes_ctx, const uint8_t *plain, size_t plain_len, size_t *enc_len); \
/****************************************************************************************************/


///////////////////////////////////
// `altstack_module` module definition
///////////////////////////////////
module(altstack_module) {
  defaults(altstack_module, CLIB_MODULE);
  PRIVATE_AES_MODULE_PROPERTIES
    PUBLIC_AES_MODULE_PROPERTIES
};
///////////////////////////////////

///////////////////////////////////
// `altstack_module` module prototypes
///////////////////////////////////
static int  altstack_module_init(module(altstack_module) * exports);
static void altstack_module_deinit(module(altstack_module) * exports);
///////////////////////////////////

///////////////////////////////////
// `altstack_module` module exports
///////////////////////////////////
exports(altstack_module) {
  .init   = altstack_module_init,
  .deinit = altstack_module_deinit,
};
///////////////////////////////////

///////////////////////////////////
// `private` module definition
///////////////////////////////////
module(private) {
  define(private, CLIB_MODULE);

  PUBLIC_AES_MODULE_PROPERTIES
};


static bool altstack_module_private_setkey(char *key){
  log_trace("altstack_module_private_setkey:%s", key);
  bool res = true;

  return(res);
}


static char * altstack_module_private_getkey(){
  log_trace("altstack_module_private_getkey");
  char *res = "OK";

  return(strdup(res));
}

// `private` module exports
exports(private) {
  defaults(private, CLIB_MODULE_DEFAULT),
  .getkey = altstack_module_private_getkey,
  .setkey = altstack_module_private_setkey,
};


static bool altstack_module_setkey(char *key){
  log_trace("altstack_module_setkey:%s", key);
  return(require(private)->setkey(key));
}


static char * altstack_module_getkey(){
  log_trace("altstack_module_getkey");
  return(require(private)->getkey());
}


// `altstack_module` module initializer
static int altstack_module_init(module(altstack_module) *exports) {
  void debug_private(){
    if (0 != exports->private) {
      log_info(AC_RESETALL AC_BOLD AC_REVERSED AC_YELLOW "init> NON PRIVATE MODE" AC_RESETALL);
    }else{
      log_info(AC_RESETALL AC_BOLD AC_REVERSED AC_BLUE "init> PRIVATE MODE" AC_RESETALL);
    }
  }

/****************************************/
  debug_private();
/****************************************/
  exports->getkey  = altstack_module_getkey;
  exports->setkey  = altstack_module_setkey;
  exports->private = require(private);

  require(private)->setkey(DEFAULT_KEY);
  log_info(AC_RESETALL AC_BOLD AC_REVERSED AC_MAGENTA "private key:%s", AC_RESETALL, exports->getkey());
/****************************************/
  debug_private();
/****************************************/


  return(0);
}


// `altstack_module` module deinitializer
static void altstack_module_deinit(module(altstack_module) *exports) {
  log_trace("altstack_module_deinit()");
  clib_module_free((module(private) *) exports->private);
}

