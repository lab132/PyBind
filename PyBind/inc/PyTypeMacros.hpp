#pragma once

#include "PyTypeBuilder.hpp"

#define PY_BIND_FUNCTION_NAMED( function, name ) pyb::Bind(&function).Bind<&function>(name)
#define PY_BIND_FUNCTION( function ) PY_BIND_FUNCTION_NAMED( function, #function)
