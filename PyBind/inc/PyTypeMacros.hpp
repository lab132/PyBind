#pragma once

#include "PyTypeBuilder.hpp"

#define PY_BIND_FUNCTION( function ) pyb::Bind(&function).Bind<&function>()
