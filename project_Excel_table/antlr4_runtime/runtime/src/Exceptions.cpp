/* Copyright (c) 2012-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

#include "Exceptions.h"

#include <utility>
#include <string>
#include <exception>

using namespace antlr4;

RuntimeException::RuntimeException(std::string msg) :  _message(std::move(msg)) {
}

const char* RuntimeException::what() const noexcept {
  return _message.c_str();
}

//------------------ IOException ---------------------------------------------------------------------------------------

IOException::IOException(std::string msg) :  _message(std::move(msg)) {
}

const char* IOException::what() const noexcept {
  return _message.c_str();
}

//------------------ IllegalStateException -----------------------------------------------------------------------------

IllegalStateException::~IllegalStateException() = default;

//------------------ IllegalArgumentException --------------------------------------------------------------------------

IllegalArgumentException::~IllegalArgumentException() = default;

//------------------ NullPointerException ------------------------------------------------------------------------------

NullPointerException::~NullPointerException() = default;

//------------------ IndexOutOfBoundsException -------------------------------------------------------------------------

IndexOutOfBoundsException::~IndexOutOfBoundsException() = default;

//------------------ UnsupportedOperationException ---------------------------------------------------------------------

UnsupportedOperationException::~UnsupportedOperationException() = default;

//------------------ EmptyStackException -------------------------------------------------------------------------------

EmptyStackException::~EmptyStackException() = default;

//------------------ CancellationException -----------------------------------------------------------------------------

CancellationException::~CancellationException() = default;

//------------------ ParseCancellationException ------------------------------------------------------------------------

ParseCancellationException::~ParseCancellationException() = default;
