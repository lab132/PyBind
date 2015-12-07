#pragma once

#include <Python.h>
#include <string>
#if defined(DEBUG) || defined(_DEBUG)

#ifndef PYB_ASSERT
#define PYB_ASSERT(...) assert(__VA_ARGS__)

#endif // !PYB_ASSERT

#else
#define PYB_ASSERT(...)
#endif // DEBUG

