#include "proto.h"

struct command * store_uncompressed (const unsigned char * data, const unsigned char * bitflipped, unsigned short * size, unsigned flags) {
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
