#include "proto.h"

int main (int argc, char ** argv) {
  struct options options = get_options(argc, argv);
  unsigned short size;
  unsigned char * file_buffer = read_file_into_buffer(options.input, &size);
  struct command * compressed;
  compressed = compress(file_buffer, &size, options.method);
  if (options.mode)
    write_commands_to_textfile(options.output, compressed, size, file_buffer, options.alignment);
  else
    write_commands_to_file(options.output, compressed, size, file_buffer, options.alignment);
  free(file_buffer);
  free(compressed);
  return 0;
}

struct command * compress (const unsigned char * data, unsigned short * size, unsigned method) {
  unsigned char * bitflipped = malloc(*size);
  unsigned current;
  for (current = 0; current < *size; current ++) bitflipped[current] = bit_flipping_table[data[current]];
  const struct compressor * compressor = compressors;
  struct command * result;
  if (method < COMPRESSION_METHODS) {
    while (method >= compressor -> methods) method -= (compressor ++) -> methods;
    result = compressor -> function(data, bitflipped, size, method);
  } else {
    struct command * compressed_sequences[COMPRESSION_METHODS];
    unsigned short lengths[COMPRESSION_METHODS];
    unsigned flags = compressor -> methods;
    for (current = 0; current < COMPRESSION_METHODS; current ++) {
      lengths[current] = *size;
      if (!flags) flags = (++ compressor) -> methods;
      compressed_sequences[current] = compressor -> function(data, bitflipped, lengths + current, -- flags);
    }
    result = select_command_sequence(compressed_sequences, lengths, COMPRESSION_METHODS, size);
    for (current = 0; current < COMPRESSION_METHODS; current ++) free(compressed_sequences[current]);
  }
  free(bitflipped);
  return result;
}
