#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define COMPRESSION_METHODS 73 /* sum of all values for the methods field in compressors */
#define MAX_FILE_SIZE 32768

struct command {
  unsigned command: 3;
  unsigned count:  12;
  signed value:    17;
};

struct compressor {
  unsigned methods;
  struct command * (* function) (const unsigned char *, const unsigned char *, unsigned short *, unsigned);
};

struct options {
  const char * input;
  const char * output;
  unsigned char mode;
  unsigned char alignment;
};

// global.c
extern const struct compressor compressors[];

// main.c
int main(int, char **);
struct command * compress(const unsigned char *, unsigned short *);

// merging.c
struct command * select_command_sequence(struct command **, const unsigned short *, unsigned, unsigned short *);
struct command * merge_command_sequences(const struct command *, unsigned short, const struct command *, unsigned short, unsigned short *);

// nullcomp.c
struct command * store_uncompressed(const unsigned char *, const unsigned char *, unsigned short *, unsigned);

// options.c
struct options get_options(int, char **);
unsigned parse_numeric_option_argument(char ***, unsigned);
void usage(const char *);

// output.c
void write_commands_to_textfile(const char *, const struct command *, unsigned, const unsigned char *, unsigned char);
void write_command_to_textfile(FILE *, struct command, const unsigned char *);
void write_commands_to_file(const char *, const struct command *, unsigned, const unsigned char *, unsigned char);
void write_command_to_file(FILE *, struct command, const unsigned char *);

// packing.c
void optimize(struct command *, unsigned short);
void repack(struct command **, unsigned short *);

// srchcomp.c
struct command * try_compress(const unsigned char *, const unsigned char *, unsigned short *, unsigned);
struct command find_best_copy(const unsigned char *, unsigned short, unsigned short, const unsigned char *, unsigned);
unsigned short scan_forwards(const unsigned char *, unsigned short, const unsigned char *, unsigned short, short *);
unsigned short scan_backwards(const unsigned char *, unsigned short, unsigned short, short *);
struct command find_best_repetition(const unsigned char *, unsigned short, unsigned short);

// util.c
void error_exit(int, const char *, ...);
unsigned char * read_file_into_buffer(const char *, unsigned short *);
void bit_flip(const unsigned char *, unsigned short, unsigned char *);
struct command pick_best_command(unsigned, struct command, ...);
int is_better(struct command, struct command);
short command_size(struct command);
unsigned short compressed_length(const struct command *, unsigned short);
