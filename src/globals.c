#include <stdio.h>
#include <string.h>

/*******************/
module(altstack_module) * cm_altstack;


/*******************/


/*******************/
void module_pre(void) {
  log_set_level(MODULE_LOG_LEVEL);
  cm_altstack = require(altstack_module);
}


/*******************/


/*******************/
void module_post(void) {
  log_debug("altstack unload>");
  clib_module_free(cm_altstack);
}


/*******************/


/*******************/
