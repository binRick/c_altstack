/*******************/
enum SUB_COMMAND_INDEXES {
  CMD_ECHO      = 0,
  CMD_ALTSTACK  = 1,
  CMD_WRAP_MAIN = 2,
};
/*******************/
#define TEST_COMMANDS                                                 \
  { "echo no args", CMD_ECHO, { NULL }, cm_wrap_main },               \
  { "echo args=OK", CMD_ECHO, { "OK" }, parse_opts },                 \
  { "altstack no args", CMD_ALTSTACK, { NULL }, parse_opts },         \
  { "altstack 150 250", CMD_ALTSTACK, { "150", "250" }, parse_opts }, \
/*******************/
#define COMMANDS                                     \
  { CMD_ECHO, "exec", cmd_exec, },                   \
  { CMD_ALTSTACK, "altstack", cmd_altstack, },       \
  { CMD_WRAP_MAIN, "wrap", CM_ALTSTACK_WRAP_MAIN, }, \
/*******************/

