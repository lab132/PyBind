#pragma once


namespace pyb
{
  inline
    Dictionary::Dictionary(const Object & obj) :
    m_Dictionary(obj)
  {
  }

  inline
    Dictionary pyb::Dictionary::FromObject(const Object & obj)
  {
    PYB_ASSERT(obj.IsValid() && obj.IsDictionary());

    return Dictionary(obj);
  }

  inline
    bool Dictionary::ContainsKey(const Object & obj) const
  {
    return PyDict_Contains(m_Dictionary.ObjectPtr(), obj.ObjectPtr());
  }

  inline
    bool Dictionary::ContainsKey(const std::string & string) const
  {
    Object obj = Object::FromNewRef(Py_BuildValue("s", string.c_str()));

    return ContainsKey(obj);
  }

  inline
    Object Dictionary::GetItem(const Object & key) const
  {
    return Object::FromBorrowed(PyDict_GetItem(m_Dictionary.ObjectPtr(), key.ObjectPtr()));
  }

  inline Object Dictionary::GetItem(const std::string & key) const
  {
    return Object::FromBorrowed(PyDict_GetItemString(m_Dictionary.ObjectPtr(), key.c_str()));
  }

  inline bool Dictionary::SetObject(const Object & key, const Object & value) const
  {
    int result = PyDict_SetItem(m_Dictionary.ObjectPtr(), key.ObjectPtr(), value.ObjectPtr());
    return result == 0;
  }

  inline bool Dictionary::SetObject(const std::string & key, const Object & value) const
  {
    int result = PyDict_SetItemString(m_Dictionary.ObjectPtr(), key.c_str(), value.ObjectPtr());
    return result == 0;
  }

  inline
    void Dictionary::Clear()
  {
    PyDict_Clear(m_Dictionary.ObjectPtr());
  }
  template<typename T>
  inline
    bool Dictionary::SetItem(const std::string & key, T value)
  {
    Object val = BuildValue<T>(value);
    return SetObject(key, val);
  }
}
