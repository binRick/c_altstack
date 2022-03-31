/*******************/
#define LOG_TIME_ENABLED    false
/*******************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/*******************/
#include "include/module_imports.h"
/*******************/
#include "src/deps.c"
/*******************/
#include "./flag_subcommands.c"
/*******************/
#define VERSION    "v1.0.0"
/*******************/
int requests = 5000;
int        concurrency = 10;
const char *url        = ":3000";
/*******************/
static const struct {
  unsigned int id;
  char         name[8];
  int          (*cmd)(char **);
} cmds[] = {
  COMMANDS
};


/*******************/


/*******************/
int parse_opts(int argc, char **argv) {
  flag_int(&requests, "requests", "Number of total requests");
  flag_int(&concurrency, "concurrency", "Number of concurrent requests");
  flag_str(&url, "url", "Target url");
  flag_parse(argc, argv, VERSION);

  log_info("     requests: %d", requests);
  log_info("  concurrency: %d", concurrency);
  log_info("          url: %s", url);

  dbge(url, % s);
  dbge(requests, % d);
  dbge(concurrency, % d);

  log_debug("OK");
  return(0);
}
/*******************/


int main(int argc, char **argv){
  int             i, option, ncmds = sizeof(cmds) / sizeof(*cmds);
  char            **subargv;
  struct optparse options;

  (void)argc;


  optparse_init(&options, argv);
  while ((option = optparse(&options, "h")) != -1) {
    switch (option) {
    case 'h':
      subcommands_usage(stdout);
      return(0);

    case '?':
      subcommands_usage(stderr);
      log_error("%s: %s\n", argv[0], options.errmsg);
      return(1);
    }
  }

  subargv = argv + options.optind;
  if (!subargv[0]) {
    log_error("%s: missing subcommand\n", argv[0]);
    subcommands_usage(stderr);
    return(1);
  }

  for (i = 0; i < ncmds; i++) {
    if (!strcmp(cmds[i].name, subargv[0])) {
      int res = cmds[i].cmd(subargv);
      log_debug("res:%d|name:%s|i:%d", res, cmds[i].name, i);
      return(res);
    }
  }
  log_error("%s: invalid subcommand: %s\n", argv[0], subargv[0]);
} /* main */

/*******************/
static const struct {
  char *name;
  int  subcommand;
  char **args;
  int  (*cmd)(const int argc, const char **argv);
} test_cmds[] = {
  TEST_COMMANDS
};
/*******************/

