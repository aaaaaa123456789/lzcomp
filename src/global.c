#include "proto.h"

const struct compressor compressors[] = {
  {.methods = 72, .function = &try_compress},      //  0-71
  {.methods =  1, .function = &store_uncompressed} // 72
};
