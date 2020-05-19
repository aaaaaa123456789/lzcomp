#include "proto.h"

struct command * try_compress_multi_pass (const unsigned char * data, const unsigned char * flipped, unsigned short * size, unsigned flags) {
  struct command * result = calloc(*size, sizeof(struct command));
  unsigned char * reversed = malloc(*size);
  short * sources = malloc(*size * sizeof(short));
  unsigned short pos, next, current = 0;
  for (pos = 0; pos < *size; pos ++) {
    reversed[pos] = data[*size - 1 - pos];
    sources[pos] = -1;
  }
  for (pos = (flags & 1); pos < *size; pos += (result[pos].count >= 64) ? result[pos].count : 1) {
    result[pos] = pick_command_for_position(data, flipped, reversed, sources, *size, pos, flags);
    if ((result[pos].command >= 4) || (result[pos].count < 64)) sources[current ++] = pos;
  }
  free(reversed);
  free(sources);
  for (pos = 0; pos < *size; pos ++) {
    for (current = 1; current < result[pos].count; current ++) if (result[pos + current].count > result[pos].count) {
      result[pos].count = current;
      if ((result[pos].command == 2) && (current & 1) && !(flags & 2)) result[pos].count --;
    }
    if (result[pos].count <= command_size(result[pos])) result[pos] = (struct command) {.command = 0, .count = 0};
  }
  for (pos = 0; pos < *size; pos ++)
    if (!result[pos].command) {
      for (current = 1; (current < 1024) && ((pos + current) < *size); current ++) if (result[pos + current].command) break;
      result[pos] = (struct command) {.command = 0, .count = current, .value = pos};
    } else if (result[pos].count > 1024) {
      result[pos + 1024] = result[pos];
      result[pos + 1024].count -= 1024;
      if ((result[pos + 1024].command >= 4) && (result[pos + 1024].value >= 0))
        result[pos + 1024].value += (result[pos].command == 6) ? -1024 : 1024;
      result[pos].count = 1024;
    }
  for (next = pos = 0; pos < *size; pos ++)
    if (pos == next)
      next += result[pos].count;
    else
      result[pos].command = 7;
  repack(&result, size);
  return result;
}

struct command pick_command_for_position (const unsigned char * data, const unsigned char * flipped, const unsigned char * reversed,
                                          const short * sources, unsigned short length, unsigned short position, unsigned flags) {
  struct command result = pick_repetition_for_position(data, length, position, flags);
  if (result.count >= 64) return result;
  unsigned char p;
  for (p = 0; p < 3; p ++) {
    struct command temp = pick_copy_for_position(data, p[(const unsigned char * []) {data, flipped, reversed}], sources, p + 4, length, position, flags);
    if (temp.command == 7) continue;
    if (temp.count > result.count) result = temp;
  }
  if ((result.command >= 4) && (result.value >= (position - 128))) result.value -= position;
  return result;
}

struct command pick_repetition_for_position (const unsigned char * data, unsigned short length, unsigned short position, unsigned flags) {
  unsigned short p;
  if (data[position]) {
    if ((position + 1) >= length) return (struct command) {.command = 1, .count = 1, .value = data[position]};
    struct command result;
    if (!(flags & 8) && (data[position] == data[position + 1]))
      result = (struct command) {.command = 1, .value = data[position]};
    else
      result = (struct command) {.command = 2, .value = data[position] | (data[position + 1] << 8)};
    for (p = 1; ((position + p) < length) && (p < LOOKAHEAD_LIMIT); p ++) if (data[position + p] != data[position + (p & 1)]) break;
    result.count = p;
    return result;
  } else {
    for (p = position + 1; (p < length) && (p < (position + LOOKAHEAD_LIMIT)); p ++) if (data[p]) break;
    return (struct command) {.command = 3, .count = p - position};
  }
}

struct command pick_copy_for_position (const unsigned char * data, const unsigned char * reference, const short * sources,
                                       unsigned char command_type, unsigned short length, unsigned short position, unsigned flags) {
  struct command result = {.command = 7, .count = (flags & 4) ? 4 : 3};
  if (length < 3) return result;
  unsigned refpos, count;
  const unsigned char * current;
  unsigned char buffer[6] = {0};
  memcpy(buffer, reference + length - 3, 3);
  while (*sources >= 0) {
    refpos = *(sources ++);
    if (command_type == 6) refpos = length - 1 - refpos;
    if (refpos >= (length - 3))
      current = buffer + refpos - (length - 3);
    else
      current = reference + refpos;
    if (memcmp(data + position, current, 4)) continue;
    for (count = 4; (count < (length - position)) && (count < (length - refpos)); count ++) if (data[position + count] != current[count]) break;
    if (count > (length - refpos)) count = length - refpos;
    if (count > (length - position)) count = length - position;
    if (result.count > count) continue;
    result = (struct command) {.command = command_type, .count = count, .value = sources[-1]};
  }
  return result;
}
