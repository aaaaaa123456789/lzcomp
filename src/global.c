#include "proto.h"

const struct compressor compressors[] = {
  /* NOTE: the "flags" field for each compressor will be set to the chosen/current method number minus the base
     number for that particular compressor. That means that each compressor will use a zero-based flags value. */
  {.methods = 72, .function = &try_compress_single_pass}, //  0-71
  {.methods =  2, .function = &store_uncompressed},       // 72-73
  {.methods =  6, .function = &try_compress_repetitions}, // 74-79
  {.methods = 16, .function = &try_compress_multi_pass}   // 80-95
};
