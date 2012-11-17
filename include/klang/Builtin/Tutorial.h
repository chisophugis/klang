//===--- Tutorial.h - -------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the builtin functions.
//
//===----------------------------------------------------------------------===//

#ifndef KLANG_TUTORIAL_H
#define KLANG_TUTORIAL_H

//===----------------------------------------------------------------------===//
// "Library" functions that can be "extern'd" from user code.
//===----------------------------------------------------------------------===//

/// putchard - putchar that takes a double and returns 0.
#ifdef __cplusplus
extern "C" {
#endif
  double putchard(double X);
  double printd(double X);
#ifdef __cplusplus
}
#endif
#endif

