#include "proto.h"

struct options get_options (int argc, char ** argv) {
  struct options result = {.input = NULL, .output = NULL, .mode = 0, .alignment = 0, .method = COMPRESSION_METHODS};
  const char * program_name = *argv;
  for (argv ++; *argv; argv ++) {
    if (**argv != '-') break;
    if (!strcmp(*argv, "--")) {
      argv ++;
      break;
    } else if (!(strcmp(*argv, "--text") && strcmp(*argv, "-t")))
      result.mode = 1;
    else if (!(strcmp(*argv, "--binary") && strcmp(*argv, "-b")))
      result.mode = 0;
    else if (!(strcmp(*argv, "--uncompress") && strcmp(*argv, "-u")))
      result.mode = 2;
    else if (!(strcmp(*argv, "--dump") && strcmp(*argv, "-d")))
      result.mode = 3;
    else if (!(strcmp(*argv, "--align") && strncmp(*argv, "-a", 2)))
      result.mode = parse_numeric_option_argument(&argv, 12);
    else if (!(strcmp(*argv, "--method") && strncmp(*argv, "-m", 2)))
      result.method = parse_numeric_option_argument(&argv, COMPRESSION_METHODS - 1);
    else if (!(strcmp(*argv, "--optimize") && strcmp(*argv, "-o")))
      result.method = COMPRESSION_METHODS;
    else
      error_exit(3, "unknown option: %s", *argv);
  }
  if (!*argv) usage(program_name);
  result.input = *argv;
  result.output = argv[1];
  return result;
}

unsigned parse_numeric_option_argument (char *** alp, unsigned limit) {
  // alp: argument list pointer (i.e., address of the current value of argv after indexing)
  // will point at the last consumed argument on exit (since the caller will probably increment it once more)
  const char * option;
  const char * value;
  char option_buffer[] = "-?";
  if (1[**alp] == '-') {
    option = *((*alp) ++);
    value = **alp;
  } else {
    option_buffer[1] = 1[**alp];
    option = option_buffer;
    value = **alp + 2;
  }
  if (!(value && *value)) error_exit(3, "option %s requires an argument", option);
  char * error;
  unsigned long result = strtoul(value, &error, 10);
  if (*error) error_exit(3, "invalid argument to option %s", option);
  if (result > limit) error_exit(3, "argument to option %s must be between 0 and %u", option, limit);
  return result;
}

void usage (const char * program_name) {
  // ... (TODO: update for new options)
  fprintf(stderr, "Usage: %s [<options>] <source file> [<compressed output>]\n\n", program_name);
  fputs("Options:\n", stderr);
  fputs("    --text    Output the command stream as text.\n", stderr);
  fputs("    --binary  Output the command stream as binary data (default).\n", stderr);
  fputs("    --        End of option list.\n", stderr);
  exit(3);
}
