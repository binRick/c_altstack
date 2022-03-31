/*******************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/*******************/
//#include "include/altstack.h"
/*******************/
#include "test_altstack_commands.c"
/*******************/


static void subcommands_usage(FILE *f) {
  fprintf(f,
          AC_RESETALL AC_MAGENTA "usage: example [-h] "
          AC_RESETALL AC_RED AC_BOLD "<%s|%s|%s>"
          AC_RESETALL AC_MAGENTA " [OPTION]..."
          AC_RESETALL "\n",
          "exec",
          "altstack",
          "cm_wrap_main"
          );
}


static int cmd_exec(char **argv){
  int             i, option;
  bool            newline = true;
  struct optparse options;

  optparse_init(&options, argv);
  options.permute = 0;
  while ((option = optparse(&options, "hn")) != -1) {
    switch (option) {
    case 'h':
      puts("usage: exec [-hn] [ARG]...");
      return(0);

    case 'n':
      log_debug("NEWLINE DISABLED");
      newline = false;
      break;
    case '?':
      log_error("%s: %s\n", argv[0], options.errmsg);
      return(1);
    }
  }
  argv += options.optind;

  for (i = 0; argv[i]; i++) {
    log_info(AC_RESETALL AC_BLUE AC_REVERSED AC_BOLD "%s%s" AC_RESETALL, i  ? " " : "", argv[i]);
  }
  if (newline) {
    log_debug("Adding Newline!");
    putchar('\n');
  }else{
    log_debug("Skipping Newline!");
  }

  fflush(stdout);
  return(!!ferror(stdout));
}


static int cmd_time1(char **argv){
  log_debug("..........");
}


static int cmd_time2(char **argv){
  log_debug("..........");
}


static int cmd_altstack(char **argv){
  int             i, option;
  struct optparse options;

  optparse_init(&options, argv);
  while ((option = optparse(&options, "h")) != -1) {
    switch (option) {
    case 'h':
      puts("usage: altstack [-h] [NUMBER]...");
      return(0);

    case '?':
      log_error("%s: %s\n", argv[0], options.errmsg);
      return(1);
    }
  }

  for (i = 0; argv[i]; i++) {
    if (sleep(atoi(argv[i]))) {
      return(1);
    }
  }
  return(0);
}
