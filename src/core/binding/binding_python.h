#pragma once

#include <xmemory>

#ifdef _DEBUG
#  undef _DEBUG // Don't let Python force the debug library just because we're debugging.
#  define DEBUG_UNDEFINED_FROM_WRAP_PYTHON_H
#endif

#define PY_SSIZE_T_CLEAN
#include "Python.h"

#ifdef DEBUG_UNDEFINED_FROM_WRAP_PYTHON_H
# undef DEBUG_UNDEFINED_FROM_WRAP_PYTHON_H
# define _DEBUG
#endif