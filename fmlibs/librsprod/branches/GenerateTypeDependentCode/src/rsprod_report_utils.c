

#include <stdio.h>
#include <stdarg.h>
#include "rsprod_intern.h"
#include "runtime_config.h"
#include "rsprod_report_utils.h"

void rsprod_echo(FILE *where,char *what,...) {
   if (librsprod_echo_mode == LIBRSPROD_VERBOSE) {
      va_list ap;
      va_start(ap,what);
      vfprintf(where, what, ap);
      va_end (ap);
   }
}
