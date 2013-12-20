#include <gmp.h>

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#define INITIAL_BITS 64
#define MAX_STACK 512
static int s_nstack;
static int s_highwater;
static mpf_t s_stack[MAX_STACK];

typedef __mpf_struct *mpf_pt;

static mpf_pt stack_next(void) {
   assert(s_nstack + 1 < MAX_STACK);

   if (s_nstack == s_highwater) {
      mpf_init2(s_stack[s_highwater++], INITIAL_BITS);
   }
   return s_stack[s_nstack];
}

static mpf_pt stack_top(void) {
   assert(s_nstack > 0);
   return s_stack[s_nstack - 1];
}

static mpf_pt stack_top2(void) {
   assert(s_nstack > 1);
   return s_stack[s_nstack - 2];
}

static void build_prompt(char *buf, size_t sz, mpf_t *items, int nitems) {
   char *at, *end;
   int i;

   /* we need at least *some* space */
   assert(sz >= 16);

   at = buf;
   end = buf + sz;

   /* display at most 4 items */
   if (nitems > 4) {
      items = items + (nitems - 4);
      nitems = 4;

      strcpy(at, "... ");
      at += 4;
   }

   for (i = 0; i < nitems; ++i) {
      assert(at != end);
      if (i > 0) {
         *at++ = ' ';
      }
      assert(at != end);
      int len = gmp_snprintf(at, (end - at), "%Fg", items[i]);
      assert(len < (end - at));
      at += len;
   }

   assert((end - at) >= 3);
   *at++ = '>';
   *at++ = ' ';
   *at++ = '\0';
}

static void strip_whitespace(char *cmd) {
   char *begin, *end;

   if (*cmd == '\0') { return; }

   end = cmd + strlen(cmd);

   begin = cmd;
   while (isspace(*begin)) { ++begin; }

   --end;
   while ((end != begin) && isspace(*end)) { --end; }
   if (!isspace(*end)) {
      ++end;
   }

   memmove(cmd, begin, (end - begin));
   cmd[end - begin] = '\0';
}

typedef void (GmpBinOpF)(mpf_ptr, mpf_srcptr, mpf_srcptr);

static void run_binop(GmpBinOpF op) {
   if (s_nstack >= 2) {
      op(stack_top2(), stack_top2(), stack_top());
      --s_nstack;
   } else {
      printf("error: stack underflow\n");
   }
}

static void print_help(FILE *to) {
   fputs("Commands:\n"
         "  + - * /   Arithmetic ops\n"
         "  d         Duplicate top of stack\n"
         "  s         Swap top two items\n"
         "  x         Pop stack\n"
         "  c         Clear stack\n"
         "  q         Quit\n",
         to);
}

static void process_command(const char *cmd) {
   if (strcmp(cmd, "+") == 0) {
      run_binop(&mpf_add);
   } else if (strcmp(cmd, "-") == 0) {
      run_binop(&mpf_sub);
   } else if (strcmp(cmd, "*") == 0) {
      run_binop(&mpf_mul);
   } else if (strcmp(cmd, "/") == 0) {
      run_binop(&mpf_div);
   } else if (strcmp(cmd, "d") == 0) {
      if (s_nstack > 0) {
         if (s_nstack + 1 < MAX_STACK) {
            mpf_set(stack_next(), stack_top());
            ++s_nstack;
         } else {
            printf("error: stack overflow\n");
         }
      }
   } else if (strcmp(cmd, "s") == 0) {
      mpf_swap(stack_top(), stack_top2());
   } else if (strcmp(cmd, "x") == 0) {
      if (s_nstack > 0) {
         --s_nstack;
      }
   } else if (strcmp(cmd, "c") == 0) {
      s_nstack = 0;
   } else if (strcmp(cmd, "?") == 0 || strcmp(cmd, "help") == 0) {
      print_help(stdout);
   } else {
      if (s_nstack + 1 < MAX_STACK) {
         mpf_pt next = stack_next();
         int ret = mpf_set_str(next, cmd, 10);
         if (ret == 0) {
            ++s_nstack;
         } else {
            printf("could not interpret value '%s'\n", cmd);
         }
      } else {
         printf("error: stack overflow\n");
      }
   }
}

static void run_interactive(void) {
   char prompt[512];
   char *input = NULL;

   do {
      build_prompt(prompt, sizeof(prompt), s_stack, s_nstack);
      input = readline(prompt);
      if (!input) { break; }
      strip_whitespace(input);
      if (*input) {
         if (strcmp(input, "q") == 0) { break; }
         process_command(input);
      }
      free(input);
   } while (1);

   if (input) {
      free(input);
   } else {
      fputs("\n", stdout);
   }
}

int main(void) {
   int i;
   s_nstack = 0;
   s_highwater = 0;

   run_interactive();

   for (i = 0; i < s_highwater; ++i) {
      mpf_clear(s_stack[i]);
   }

   return EXIT_SUCCESS;
}

/* vim: set ts=8 sts=3 sw=3 et ai: */
