/*******************/

#define PRINT_AES_KEY()                  {                             \
    char *k = GET_AES_KEY();                                           \
    LOG(LOG_AES_STYLE "%db Current Key:%s" AC_RESETALL, strlen(k), k); \
}                                                                      \
/*******************/
#define TEST_SET_AES_KEY(NEW_KEY)        {                                                                                \
    int res = SET_AES_KEY(NEW_KEY);                                                                                       \
    LOG(LOG_AES_STYLE "AES Key Set from %db to %db string: %d" AC_RESETALL, strlen(NEW_KEY), strlen(GET_AES_KEY()), res); \
}                                                                                                                         \
/*******************/
#define POST_MODULE \
  module_post();    \
/*******************/
#define PRE_MODULE \
  module_pre();    \
/*******************/
#define MODULE_TEST_SET_KEY_HANDLER()    {    \
    TEST_SET_AES_KEY(DEFAULT_CRYPTO_AES_KEY); \
}                                             \
/*******************/
#define TEST_MODULE                                         \
  IF_EXEC(MODULE_TEST_SET_KEY, MODULE_TEST_SET_KEY_HANDLER) \
  IF_EXEC(MODULE_TEST_GET_KEY, PRINT_AES_KEY)               \
/*******************/


/*******************/
#define MODULE_HOOK                    \
  PRINT_AES_KEY();                     \
  SET_AES_KEY(DEFAULT_CRYPTO_AES_KEY); \
  PRINT_AES_KEY();                     \
/*******************/
#define MODULE_LIFECYCLE \
  PRE_MODULE             \
  MODULE_HOOK            \
  TEST_MODULE            \
  POST_MODULE            \
/*******************/
