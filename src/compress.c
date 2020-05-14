#include "proto.h"

struct command * compress (const unsigned char * data, unsigned short * size) {
  unsigned char * bitflipped = malloc(*size);
  bit_flip(data, *size, bitflipped);
  struct command * compressed_sequences[COMPRESSION_METHODS + 1];
  unsigned short lengths[COMPRESSION_METHODS + 1];
  unsigned current;
  for (current = 0; current < COMPRESSION_METHODS; current ++) {
    lengths[current] = *size;
    compressed_sequences[current] = try_compress(data, bitflipped, lengths + current, current);
  }
  free(bitflipped);
  lengths[COMPRESSION_METHODS] = *size;
  compressed_sequences[COMPRESSION_METHODS] = store_uncompressed(lengths + COMPRESSION_METHODS);
  struct command * result = select_command_sequence(compressed_sequences, lengths, COMPRESSION_METHODS + 1, size);
  for (current = 0; current <= COMPRESSION_METHODS; current ++) free(compressed_sequences[current]);
  return result;
}

struct command * store_uncompressed (unsigned short * size) {
  unsigned short position, block, remainder = *size;
  struct command * result = NULL;
  *size = 0;
  for (position = 0; remainder; position += block, remainder -= block) {
    block = (remainder > 1024) ? 1024 : remainder;
    if ((block <= 64) && (block > 32)) block = 32;
    result = realloc(result, sizeof(struct command) * (1 + *size));
    result[(*size) ++] = (struct command) {.command = 0, .count = block, .value = position};
  }
  return result;
}
