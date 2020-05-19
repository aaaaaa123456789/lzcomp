#include "proto.h"

struct command * store_uncompressed (const unsigned char * data, const unsigned char * bitflipped, unsigned short * size, unsigned flags) {
  unsigned short position, block, remainder = *size;
  struct command * result = NULL;
  *size = 0;
  for (position = 0; remainder; position += block, remainder -= block) {
    block = (remainder > MAX_COMMAND_COUNT) ? MAX_COMMAND_COUNT : remainder;
    if (!(flags & 1) && (block <= (2 * SHORT_COMMAND_COUNT)) && (block > SHORT_COMMAND_COUNT)) block = SHORT_COMMAND_COUNT;
    result = realloc(result, sizeof(struct command) * (1 + *size));
    result[(*size) ++] = (struct command) {.command = 0, .count = block, .value = position};
  }
  return result;
}
