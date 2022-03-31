/* Licensed under Apache License v2.0 - see LICENSE file for details */
#include "altstack.h"

#include <assert.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static __thread struct altstack_state {
  char     ebuf[ALTSTACK_ERR_MAXLEN];
  unsigned elen;
  jmp_buf  jmp;
  void     *rsp_save[2];
  rlim_t   max;
  void     *(*fn)(void *);
  void     *arg, *out;
} state;

#define bang(x)                                                   \
  (state.elen += snprintf(state.ebuf + state.elen,                \
                          sizeof(state.ebuf) - state.elen,        \
                          "%s(altstack@%d) %s%s%s",               \
                          state.elen  ? "; " : "", __LINE__, (x), \
                          errno ? ": " : "",                      \
                          errno ? strerror(errno) : ""))


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


int altstack(rlim_t max, void *(*fn)(void *), void *arg, void **out){
  long             pgsz = sysconf(_SC_PAGESIZE);
  int              ret = -1, undo = 0;
  char             *m;
  struct rlimit    rl_save;
  struct sigaction sa_save;
  int              errno_save;
  stack_t          ss_save;

  assert(max > 0 && fn);
#define ok(x, y)    ({ long __r = (long)(x); if (__r == -1) { bang(#x); if (y) goto out; } __r; })

  state.fn = fn;
  state.arg = arg;
  state.out = NULL;
  state.max = max;
  state.ebuf[state.elen = 0] = '\0';
  if (out) {
    *out = NULL;
  }

  // if the first page below the mapping is in use, we get max-pgsz usable bytes
  // add pgsz to max to guarantee at least max usable bytes
  max += pgsz;

  ok(getrlimit(RLIMIT_STACK, &rl_save), 1);
  ok(setrlimit(RLIMIT_STACK, &(struct rlimit) { state.max, rl_save.rlim_max }), 1);
  undo++;

  ok(m = mmap(NULL, max, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_GROWSDOWN | MAP_NORESERVE, -1, 0), 1);
  undo++;

  if (setjmp(state.jmp) == 0) {
    unsigned char    sigstk[SIGSTKSZ];
    stack_t          ss = { .ss_sp = sigstk, .ss_size = sizeof(sigstk) };
    struct sigaction sa = { .sa_handler = segvjmp, .sa_flags = SA_NODEFER | SA_RESETHAND | SA_ONSTACK };

    ok(sigaltstack(&ss, &ss_save), 1);
    undo++;

    sigemptyset(&sa.sa_mask);
    ok(sigaction(SIGSEGV, &sa, &sa_save), 1);
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
  }else  {
    errno = 0;
    bang("SIGSEGV caught");
    errno = EOVERFLOW;
  }

out:
  errno_save = errno;

  switch (undo) {
  case 4:
    ok(sigaction(SIGSEGV, &sa_save, NULL), 0);
  case 3:
    ok(sigaltstack(&ss_save, NULL), 0);
  case 2:
    ok(munmap(m, max), 0);
  case 1:
    ok(setrlimit(RLIMIT_STACK, &rl_save), 0);
  }

  if (errno_save) {
    errno = errno_save;
  }
  return(!ret && state.elen ? 1 : ret);
} /* altstack */


#define __cm_altstack_ok__(x)    ({ int __r = (x); if (__r == -1) err(1, #x); __r; })
/******************************************************************************************/
#define CM_ALTSTACK_OK    __cm_altstack_ok__
/******************************************************************************************/

char cm_altstack_maps[128], cm_altstack_rss[128];


static void __cm_altstack_stack_used__(void) {
  fprintf(stderr, "stack used: %ld\n", altstack_used());
}
/******************************************************************************************/
#define CM_ALTSTACK_PRINT_STACK_USED    __cm_altstack_stack_used__
#define CM_ALTSTACK_MAPS                cm_altstack_maps
#define CM_ALTSTACK_RSS                 cm_altstack_rss


/******************************************************************************************/


/******************************************************************************************/
static void *cm_wrap_fn(void *arg){
  CM_ALTSTACK_OK(system(cm_altstack_maps));

  CM_ALTSTACK_PRINT_STACK_USED();
  CM_ALTSTACK_OK(system(cm_altstack_rss));

  char p[(long)arg];

  CM_ALTSTACK_PRINT_STACK_USED();
  CM_ALTSTACK_OK(system(cm_altstack_rss));

  memset(p, 0, sizeof(p));

  CM_ALTSTACK_PRINT_STACK_USED();
  CM_ALTSTACK_OK(system(cm_altstack_rss));

  return((void *)0xaced);
}
/******************************************************************************************/
#define CM_ALTSTACK_WRAP_FUNCTION    cm_wrap_fn


/******************************************************************************************/


/******************************************************************************************/
int *cm_wrap_main(char **argv){
  long stk_max, vla_sz;
  int  ret;
  void *out;

//        assert(argc == 3);
  stk_max = strtol(argv[1], NULL, 0) * 1024 * 1024;
  vla_sz  = strtol(argv[2], NULL, 0) * 1024 * 1024;
  assert(stk_max > 0 && vla_sz > 0);

  snprintf(cm_altstack_maps, sizeof(cm_altstack_maps), "egrep '\\[stack' /proc/%d/maps", getpid());
  snprintf(cm_altstack_rss, sizeof(cm_altstack_rss), "egrep '^VmRSS' /proc/%d/status", getpid());

  CM_ALTSTACK_OK(system(cm_altstack_maps));
  CM_ALTSTACK_OK(system(cm_altstack_rss));

  ret = altstack(stk_max, CM_ALTSTACK_WRAP_FUNCTION, (void *)vla_sz, &out);

  CM_ALTSTACK_OK(system(cm_altstack_maps));
  CM_ALTSTACK_OK(system(cm_altstack_rss));

  if (ret) {
    altstack_perror();
  }
  fprintf(stderr, "altstack return: %d, CM_ALTSTACK_WRAP_FUNCTION return: %p\n", ret, out);

  return(0);
}
/******************************************************************************************/
#define CM_ALTSTACK_WRAP_MAIN    cm_wrap_main
/******************************************************************************************/
