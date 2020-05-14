#include "proto.h"

struct options get_options (int argc, char ** argv) {
  struct options result = {.input = NULL, .output = NULL, .mode = 0};
  const char * program_name = *argv;
  for (argv ++; *argv; argv ++) {
    if (strncmp(*argv, "--", 2)) break;
    if (!strcmp(*argv, "--")) {
      argv ++;
      break;
    } else if (!strcmp(*argv, "--text"))
      result.mode = 1;
    else if (!strcmp(*argv, "--binary"))
      result.mode = 0;
    else
      error_exit(3, "unknown option: %s", *argv);
  }
  if (!*argv) usage(program_name);
  result.input = *argv;
  result.output = argv[1];
  return result;
}

void usage (const char * program_name) {
  fprintf(stderr, "Usage: %s [<options>] <source file> [<compressed output>]\n\n", program_name);
  fputs("Options:\n", stderr);
  fputs("    --text    Output the command stream as text.\n", stderr);
  fputs("    --binary  Output the command stream as binary data (default).\n", stderr);
  fputs("    --        End of option list.\n", stderr);
  exit(3);
}
