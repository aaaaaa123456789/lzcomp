#include "proto.h"

int main (int argc, char ** argv) {
  struct options options = get_options(argc, argv);
  unsigned short size;
  unsigned char * file_buffer = read_file_into_buffer(options.input, &size);
  struct command * compressed = compress(file_buffer, &size);
  if (options.mode)
    write_commands_to_textfile(options.output, compressed, size, file_buffer);
  else
    write_commands_to_file(options.output, compressed, size, file_buffer);
  free(file_buffer);
  free(compressed);
  return 0;
}

void error_exit (int error_code, const char * error, ...) {
  va_list ap;
  va_start(ap, error);
  fputs("error: ", stderr);
  vfprintf(stderr, error, ap);
  fputc('\n', stderr);
  exit(error_code);
}

unsigned char * read_file_into_buffer (const char * file, unsigned short * size) {
  FILE * fp = fopen(file, "rb");
  if (!fp) error_exit(1, "could not open file %s for reading", file);
  unsigned char * buf = malloc(32769);
  int rv = fread(buf, 1, 32769, fp);
  fclose(fp);
  if (rv < 0) error_exit(1, "could not read from file %s", file);
  if (rv > 32768) error_exit(1, "file %s is too big", file);
  *size = rv;
  return buf;
}
