#pragma once

#include "PyList.hpp"

namespace pyb
{
  inline
  pyb::List::List() :
    m_PyObject()
  {
  }

  inline List::List(const Object & obj) :
    m_PyObject(obj)
  {
  }
  inline bool List::IsValid() const
  {
    return m_PyObject.IsValid() && m_PyObject.IsList();
  }
  inline List List::FromObject(const Object & obj)
  {
    PYB_ASSERT(obj.IsValid() && obj.IsList());
    return List(obj);
  }
}
