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

  inline Object List::GetItemObject(size_t index) const
  {
    PYB_ASSERT(IsValid());
    PYB_ASSERT(index < Size());
    return Object::FromBorrowed(PyList_GetItem(m_PyObject.ObjectPtr(), index));
  }

  inline void List::SetItemObject(size_t index, Object& newValue)
  {
    PYB_ASSERT(IsValid());
    PYB_ASSERT(index < Size());
    PYB_ASSERT(newValue.IsValid());
    newValue.IncrementRefCount(); // Needed to call manually, since PyList_SetItem steals a reference.
    int result = PyList_SetItem(m_PyObject.ObjectPtr(), index, newValue.ObjectPtr());
    PYB_ASSERT(result == 0);
  }

  template<typename T>
  inline void List::SetItem(size_t index, const T & item)
  {
    return SetItemObject(index, BuildValue<T>(item));
  }

  inline size_t List::Size() const
  {
    PYB_ASSERT(m_PyObject.IsValid());
    return PyList_Size(m_PyObject.ObjectPtr());
  }

  inline List List::FromObject(const Object & obj)
  {
    PYB_ASSERT(obj.IsValid());
    PYB_ASSERT(obj.IsList());
    return List(obj);
  }

  inline List List::Create()
  {
    return Object::FromNewRef(PyList_New(0)).ToList();
  }

  template<typename T>
  inline T pyb::List::GetItem(size_t index) const
  {
    Object obj = GetItemObject(index);
    PYB_ASSERT(obj.IsValid());
    return obj.ToValue<T>();
  }

  inline ListEntryRef List::operator[](size_t index) const
  {
    return ListEntryRef(index, *this);
  }

  inline
  pyb::ListEntryRef::ListEntryRef(size_t key, const List & list) :
    m_List(list),
    m_Key(key)
  {
  }

  template<typename T>
  inline void pyb::ListEntryRef::operator=(T obj)
  {
    Object createdObject = BuildValue<T>(obj);
    m_List.SetItemObject(m_Key, createdObject);
  }
  template<typename T>
  inline T ListEntryRef::GetValue() const
  {
    return m_List.GetItem<T>(m_Key);
  }
  template<typename T>
  inline ListEntryRef::operator T() const
  {
    return GetValue<T>();
  }
}
