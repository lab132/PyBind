#pragma once

#include "PyObject.hpp"

namespace pyb
{
  class List
  {
  public:

    List();
    List(const Object& obj);

    bool IsValid() const;

    Object m_PyObject;

    static List FromObject(const Object& obj);
  };
}
