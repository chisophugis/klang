
#include "klang/Builtin/Tutorial.h"
#include <cstdio>

//===----------------------------------------------------------------------===//
// "Library" functions that can be "extern'd" from user code.
//===----------------------------------------------------------------------===//

/// putchard - putchar that takes a double and returns 0.
double putchard(double X) {
  putchar((char)X);
  return 0;
}

