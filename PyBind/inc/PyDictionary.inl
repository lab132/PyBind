#include "PyDictionary.hpp"
#pragma once


namespace pyb
{
  inline
  pyb::DictionaryEntryRef::DictionaryEntryRef(const Object & key, const Dictionary& dict) :
    m_Key(key),
    m_Dict(dict)
  {
  }

  template<typename T>
  inline void pyb::DictionaryEntryRef::operator=(const T & obj)
  {
    m_Dict.SetItem<T>(m_Key, obj);
  }

  template<typename T>
  inline T DictionaryEntryRef::GetValue() const
  {
    return m_Dict.GetItem<T>(m_Key);
  }

  template<typename T>
  inline DictionaryEntryRef::operator T() const
  {
    return m_Dict.GetItem<T>(m_Key);
  }

  inline
    Dictionary::Dictionary(const Object & obj) :
    m_Dictionary(obj)
  {
  }

  inline
  Dictionary pyb::Dictionary::FromObject(const Object & obj)
  {
    PYB_ASSERT(obj.IsValid());
    PYB_ASSERT(obj.IsDictionary());

    return Dictionary(obj);
  }

  inline Dictionary Dictionary::Create()
  {
    return Object::FromNewRef(PyDict_New());
  }

  inline
  bool Dictionary::ContainsKey(const Object& obj) const
  {
    PYB_ASSERT(obj.IsValid());
    return PyDict_Contains(m_Dictionary.ObjectPtr(), obj.ObjectPtr()) != 0;
  }

  inline
  bool Dictionary::ContainsKey(const std::string& string) const
  {
    Object obj = Object::FromNewRef(Py_BuildValue("s", string.c_str()));

    return ContainsKey(obj);
  }

  inline
  bool Dictionary::DeleteItem(const Object& key) const
  {
    PYB_ASSERT(key.IsValid());
    return PyDict_DelItem(m_Dictionary.ObjectPtr(), key.ObjectPtr()) == 0;
  }

  inline
  bool Dictionary::DeleteItem(const std::string& key) const
  {
    return PyDict_DelItemString(m_Dictionary.ObjectPtr(), key.c_str()) == 0;
  }

  inline
  Object Dictionary::GetItem(const Object & key) const
  {
    PYB_ASSERT(key.IsValid());
    return Object::FromBorrowed(PyDict_GetItem(m_Dictionary.ObjectPtr(), key.ObjectPtr()));
  }

  inline Object Dictionary::GetItem(const std::string & key) const
  {
    return Object::FromBorrowed(PyDict_GetItemString(m_Dictionary.ObjectPtr(), key.c_str()));
  }

  inline bool Dictionary::SetObject(const Object & key, const Object & value) const
  {
    PYB_ASSERT(key.IsValid());
    PYB_ASSERT(value.IsValid());
    int result = PyDict_SetItem(m_Dictionary.ObjectPtr(), key.ObjectPtr(), value.ObjectPtr());
    return result == 0;
  }

  inline bool Dictionary::SetObject(const std::string & key, const Object & value) const
  {
    PYB_ASSERT(value.IsValid());
    int result = PyDict_SetItemString(m_Dictionary.ObjectPtr(), key.c_str(), value.ObjectPtr());
    return result == 0;
  }

  inline
    void Dictionary::Clear()
  {
    PyDict_Clear(m_Dictionary.ObjectPtr());
  }

  inline size_t Dictionary::Size() const
  {
    return PyDict_Size(m_Dictionary.ObjectPtr());
  }

  template<typename T>
  inline
    T Dictionary::GetItem(const std::string & key) const
  {
    Object obj = GetItem(key);
    return obj.ToValue<T>();
  }

  template<typename T>
  inline T Dictionary::GetItem(const Object & key) const
  {
    Object obj = GetItem(key);
    return obj.ToValue<T>();
  }

  inline DictionaryEntryRef Dictionary::operator[](const std::string & key)
  {
    return DictionaryEntryRef(BuildValue(key), *this);
  }

  template<typename T>
  inline
    bool Dictionary::SetItem(const std::string & key, T value)
  {
    Object val = BuildValue<T>(value);
    return SetObject(key, val);
  }
  template<typename T>
  inline bool Dictionary::SetItem(const Object & key, T value)
  {
    Object val = BuildValue<T>(value);
    return SetObject(key, val);
  }
}
