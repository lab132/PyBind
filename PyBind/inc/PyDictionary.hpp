#pragma once

#include "PyObject.hpp"

namespace pyb
{
  class Dictionary
  {
  public:

    Dictionary(const Object& obj);

    static Dictionary FromObject(const Object& obj);

    bool ContainsKey(const Object& obj) const;
    bool ContainsKey(const std::string& string) const;

  private:
    Object m_Dictionary;
  };


  inline
  Dictionary::Dictionary(const Object & obj) :
    m_Dictionary(obj)
  {
  }

  inline
  Dictionary pyb::Dictionary::FromObject(const Object & obj)
  {
    PYB_ASSERT(obj.IsValid() && obj.IsDictionary());

    return Dictionary( obj );
  }
  inline bool Dictionary::ContainsKey(const Object & obj) const
  {
    return PyDict_Contains(m_Dictionary.ObjectPtr(), obj.ObjectPtr());
  }
  inline bool Dictionary::ContainsKey(const std::string & string) const
  {
    Object obj = Object::FromNewRef(Py_BuildValue("s", string.c_str()));

    return ContainsKey(obj);
  }
}
