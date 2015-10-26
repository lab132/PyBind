#pragma once

#include "PyTypeBuilder.hpp"

#define PY_BIND_FUNCTION_NAMED( function, name ) pyb::Bind(&function).Bind<&function>(name)
#define PY_BIND_FUNCTION( function ) PY_BIND_FUNCTION_NAMED( function, #function)
#define PY_BIND_METHOD( class, member) PY_BIND_FUNCTION_NAMED( class::member, #member)
#define PY_BIND_CTOR( class, ...) pyb::CtorHelper< class >::Bind< __VA_ARGS__ >()
