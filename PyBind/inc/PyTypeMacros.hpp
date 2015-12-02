#pragma once

#include "PyTypeBuilder.hpp"

#define PY_BIND_FUNCTION_NAMED( function, name ) pyb::Bind(&function).Bind<&function>(name)
#define PY_BIND_FUNCTION( function ) PY_BIND_FUNCTION_NAMED( function, #function)
#define PY_BIND_METHOD( class, member) PY_BIND_FUNCTION_NAMED( class::member, #member)
#define PY_BIND_PROPERTY_NAMED( setFunc, getFunc, name) pyb::Bind(&setFunc,&getFunc).Bind<&setFunc,&getFunc>(name)
#define PY_BIND_PROPERTY( class, memberName) \
  PY_BIND_PROPERTY_NAMED( class##::##Set##memberName , class##::##Get##memberName, #memberName)
#define PY_BIND_CTOR( class, ...) pyb::CtorHelper< class >::Bind< __VA_ARGS__ >()
