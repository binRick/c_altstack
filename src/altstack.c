/******************************************************************************************/
#include "altstack.h"
/******************************************************************************************/
#include "../human/ms.c"
/******************************************************************************************/
#include <assert.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
/******************************************************************************************/
#define CM_ALTSTACK_STYLE           AC_REVERSED AC_BRIGHT_GREEN_BLACK AC_ITALIC
#define MICROSSECONDS_IN_SECONDS    1000000
#define CM_ALTSACK_MEM_ADDR         0xaced
#define MMAP_OPTIONS \
  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_GROWSDOWN | MAP_NORESERVE
/******************************************************************************************/
#define CM_OK_FXN(x, y)                         ({ long __r = (long)(x); if (__r == -1) { bang(#x); if (y) goto out; } __r; })
/******************************************************************************************/
#define MICROSECONDS_DURATION_TO_STRING(DUR)    { \
  (DUR > 1000 * 1000) ? return(milliseconds_to_long_string(DUR / 1000) : return milliseconds_to_string(DUR / 1000)); }
#define CM_ALTSTACK_SETUP_MEMORY_USAGE_COPY_COMMANDS                                                \
  snprintf(CM_ALTSTACK_MAPS, sizeof(CM_ALTSTACK_MAPS), "egrep '\\[stack' /proc/%d/maps", getpid()); \
  snprintf(CM_ALTSTACK_RSS, sizeof(CM_ALTSTACK_RSS), "egrep '^VmRSS' /proc/%d/status", getpid());
/******************************************************************************************/
long stk_max, vla_sz;
/******************************************************************************************/
#define __cm_altstack_ok__(x)    ({ int __r = (x); if (__r == -1) err(1, #x); __r; })
#define CM_ALTSTACK_ASSIGN_MEMORY_AND_USAGE_IN_MEGABYTES \
  stk_max = strtol(argv[1], NULL, 0) * 1024 * 1024;      \
  vla_sz  = strtol(argv[2], NULL, 0) * 1024 * 1024;
#define CM_ALTSTACK_ASSERT_MEMORY_AND_USAGE \
  assert(stk_max > 0 && vla_sz > 0);
#define bang(x)                                                   \
  (state.elen += snprintf(state.ebuf + state.elen,                \
                          sizeof(state.ebuf) - state.elen,        \
                          "%s(altstack@%d) %s%s%s",               \
                          state.elen  ? "; " : "", __LINE__, (x), \
                          errno ? ": " : "",                      \
                          errno ? strerror(errno) : ""))
/******************************************************************************************/


/******************************************************************************************/
unsigned long __ts__(){
  struct timeval tmp;

  gettimeofday(&(tmp), NULL);
  return((tmp.tv_usec) + (tmp.tv_sec * MICROSSECONDS_IN_SECONDS));
}
/******************************************************************************************/
static __thread struct altstack_state {
  char     ebuf[ALTSTACK_ERR_MAXLEN];
  unsigned elen;
  jmp_buf  jmp;
  void     *rsp_save[2];
  rlim_t   max;
  void     *(*fn)(void *);
  void     *arg, *out;
}                      state;
char                   __cm_altstack_maps__[128], __cm_altstack_rss__[128];
volatile unsigned long CM_ALTSACK_SLEEP_US = 0;


/******************************************************************************************/
void altstack_perror(void){
  fprintf(stderr, "%s\n", state.ebuf);
}


char *altstack_geterr(void){
  return(state.ebuf);
}


static void segvjmp(int signum){
  longjmp(state.jmp, 1);
}


rlim_t altstack_max(void) {
  return(state.max);
}


static ptrdiff_t rsp_save(unsigned i) {
  assert(i < 2);
  asm volatile ("movq %%rsp, %0" : "=g" (state.rsp_save[i]));
  return((char *)state.rsp_save[0] - (char *)state.rsp_save[i]);
}


void altstack_rsp_save(void) {
  rsp_save(0);
}


ptrdiff_t altstack_used(void) {
  return(rsp_save(1));
}
/******************************************************************************************/
#define CM_ALTSTACK_MICROSECONDS            __ts__
#define CM_ALTSTACK_WRAP_FUNCTION           __cm_request_memory__
#define CM_ALTSTACK_PRINT_STACK_USED        __cm_altstack_stack_used__
#define CM_ALTSTACK_MAPS                    __cm_altstack_maps__
#define CM_ALTSTACK_RSS                     __cm_altstack_rss__
#define CM_ALTSTACK_COLLECTION_FUNCTIONS    __cm_altstack_ok__
/******************************************************************************************/
#define CM_ALTSACK_USAGE_MSG                                         \
  AC_RESETALL CM_ALTSTACK_STYLE "<%d>" AC_RESETALL " "               \
  "|" AC_RESETALL CM_ALTSTACK_STYLE "Stack Limit: %s" AC_RESETALL    \
  "|" AC_RESETALL CM_ALTSTACK_STYLE "Sleep: %lums" AC_RESETALL       \
  "|" AC_RESETALL CM_ALTSTACK_STYLE "Memory Limit: %s" AC_RESETALL   \
  "|" AC_RESETALL CM_ALTSTACK_STYLE "Memory Request: %s" AC_RESETALL \
  "|"                                                                \
  , getpid()                                                         \
  , (__asu > 0) ? bytes_to_string(__asu) : "Nan"                     \
  , CM_ALTSACK_SLEEP_US                                              \
  , bytes_to_string(stk_max)                                         \
  , bytes_to_string(vla_sz)


/******************************************************************************************/
static void __cm_altstack_stack_used__(void) {
  int64_t __asu = altstack_used();

  log_debug(CM_ALTSACK_USAGE_MSG);
}
#define CM_ALTSTACK_EXECUTE_MEMORY_USAGE_COMMANDS             \
  CM_ALTSTACK_COLLECTION_FUNCTIONS(system(CM_ALTSTACK_MAPS)); \
  CM_ALTSTACK_COLLECTION_FUNCTIONS(system(CM_ALTSTACK_RSS));  \
/******************************************************************************************/


/******************************************************************************************/
static void *__cm_request_memory__(void *arg){
  char p[(long)arg];

  memset(p, 0, sizeof(p));
  CM_ALTSTACK_EXECUTE_MEMORY_USAGE_COMMANDS
  CM_ALTSTACK_PRINT_STACK_USED();

  if (CM_ALTSACK_SLEEP_US > 0) {
    usleep(CM_ALTSACK_SLEEP_US);
  }
  return((void *)CM_ALTSACK_MEM_ADDR);
}


/******************************************************************************************/


int altstack(rlim_t max, void *(*fn)(void *), void *arg, void **out){
  long             pgsz = sysconf(_SC_PAGESIZE);
  int              ret = -1, undo = 0;
  char             *m;
  struct rlimit    rl_save;
  struct sigaction sa_save;
  int              errno_save;
  stack_t          ss_save;

  assert(max > 0 && fn);

  state.fn                   = fn;
  state.arg                  = arg;
  state.out                  = NULL;
  state.max                  = max;
  state.ebuf[state.elen = 0] = '\0';
  if (out) {
    *out = NULL;
  }
  max += pgsz;
  CM_OK_FXN(getrlimit(RLIMIT_STACK, &rl_save), 1);
  CM_OK_FXN(setrlimit(RLIMIT_STACK, &(struct rlimit) { state.max, rl_save.rlim_max }), 1);

  undo++;

  CM_OK_FXN(m = mmap(NULL, max, MMAP_OPTIONS, -1, 0), 1);
  undo++;

  if (setjmp(state.jmp) == 0) {
    unsigned char    sigstk[SIGSTKSZ];
    stack_t          ss = { .ss_sp = sigstk, .ss_size = sizeof(sigstk) };
    struct sigaction sa = { .sa_handler = segvjmp, .sa_flags = SA_NODEFER | SA_RESETHAND | SA_ONSTACK };

    CM_OK_FXN(sigaltstack(&ss, &ss_save), 1);
    undo++;

    sigemptyset(&sa.sa_mask);
    CM_OK_FXN(sigaction(SIGSEGV, &sa, &sa_save), 1);
    undo++;

    asm volatile (
      "mov %%rsp, %%r10\n\t"
      "mov %1, %%rsp\n\t"
      "sub $8, %%rsp\n\t"
      "push %%r10"
      : "=r" (state.rsp_save[0])
      : "0" (m + max) : "r10", "memory");
    state.out = state.fn(state.arg);
    asm volatile ("pop %%rsp"
                  : : : "memory");
    ret = 0;
    if (out) {
      *out = state.out;
    }
  }else {
    errno = 0;
    bang("SIGSEGV caught");
    errno = EOVERFLOW;
  }

out:
  errno_save = errno;

  switch (undo) {
  case 4:
    CM_OK_FXN(sigaction(SIGSEGV, &sa_save, NULL), 0);
  case 3:
    CM_OK_FXN(sigaltstack(&ss_save, NULL), 0);
  case 2:
    CM_OK_FXN(munmap(m, max), 0);
  case 1:
    CM_OK_FXN(setrlimit(RLIMIT_STACK, &rl_save), 0);
  }
  if (errno_save) {
    errno = errno_save;
  }
  return(!ret && state.elen ? 1 : ret);
} /* altstack */


int *cm_wrap_main(char **argv){
  int ret; void *out;
  CM_ALTSTACK_ASSIGN_MEMORY_AND_USAGE_IN_MEGABYTES
  CM_ALTSTACK_ASSERT_MEMORY_AND_USAGE
  CM_ALTSTACK_SETUP_MEMORY_USAGE_COPY_COMMANDS
  CM_ALTSTACK_EXECUTE_MEMORY_USAGE_COMMANDS
  unsigned long __started = CM_ALTSTACK_MICROSECONDS();

  ret = altstack(stk_max, CM_ALTSTACK_WRAP_FUNCTION, (void *)vla_sz, &out);
  unsigned long __dur = CM_ALTSTACK_MICROSECONDS() - __started;
  char          *dur  = (__dur > 1000 * 1000) ? milliseconds_to_long_string(__dur / 1000) : milliseconds_to_string(__dur / 1000);

  log_debug("Duration:%s", dur);

  if (ret) {
    altstack_perror();
  }
  log_info(
    AC_RESETALL AC_REVERSED AC_BLUE AC_FAINT "<%d>"
    AC_RESETALL " " AC_UNDERLINE AC_MAGENTA "Altstack>"
    AC_RESETALL " " AC_REVERSED AC_BRIGHT_GREEN_BLACK "|Return:%d|"
    AC_RESETALL " " AC_REVERSED AC_BRIGHT_YELLOW_BLACK "|FxnReturn:%d|"
    AC_RESETALL " " AC_REVERSED AC_BRIGHT_MAGENTA_BLACK "|Pointer:%p|",
    getpid(),
    ret,
    out
    );

  return(0);
}
/******************************************************************************************/
#define CM_ALTSTACK_WRAP_MAIN    cm_wrap_main
/******************************************************************************************/
