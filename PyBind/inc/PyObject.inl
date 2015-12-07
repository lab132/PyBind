#include "PyObject.hpp"
#pragma once
#include "PyTypeObject.hpp"

namespace pyb
{
  inline
    Object::Object(const Object & obj) :
    m_PyObject(obj.m_PyObject)
  {
    Py_XINCREF(m_PyObject);
  }

  inline
    Object::Object(Object && other) :
    m_PyObject(other.m_PyObject)
  {
    other.m_PyObject = nullptr;
  }

  inline Object::Object() :
    m_PyObject(nullptr)
  {
  }

  inline
    Object::~Object()
  {
    Py_XDECREF(m_PyObject);
    m_PyObject = nullptr;
  }

  inline
    PyObject* pyb::Object::ObjectPtr() const
  {
    return m_PyObject;
  }

  inline bool Object::IsValid() const
  {
    return m_PyObject != nullptr;
  }

  inline
    bool Object::IsDictionary() const
  {
    return PyDict_Check(m_PyObject);
  }

  inline bool Object::IsList() const
  {
    return PyList_Check(m_PyObject);
  }

  inline
    bool Object::IsCallable() const
  {
    return PyCallable_Check(m_PyObject) != 0;
  }

  inline void Object::Invalidate()
  {
    Py_XDECREF(m_PyObject);
    m_PyObject = nullptr;
  }

  inline bool Object::IsNone() const
  {
    return m_PyObject == Py_None;
  }

  inline bool Object::IsOfType(BaseTypeObject* type) const
  {
    return PyObject_IsInstance(m_PyObject, reinterpret_cast<PyObject*> (&type->m_Binding)) == 1;
  }

  inline void Object::IncrementRefCount()
  {
    PYB_ASSERT(IsValid());
    Py_INCREF(m_PyObject);
  }

  inline void Object::DecrementRefCount()
  {
    PYB_ASSERT(IsValid());
    Py_DECREF(m_PyObject);
  }

  inline Dictionary Object::ToDictionary()
  {
    return Dictionary::FromObject(*this);
  }

  inline List Object::ToList()
  {
    return List::FromObject(*this);
  }

  inline
    Object Object::FromBorrowed(PyObject * pyObject)
  {
    Object obj;
    obj.m_PyObject = pyObject;
    Py_XINCREF(pyObject);
    return obj;
  }

  inline
    Object Object::FromNewRef(PyObject* pyObject)
  {
    Object obj;
    obj.m_PyObject = pyObject;
    return obj;
  }

  inline
    Object & pyb::Object::operator=(const Object & other)
  {
    m_PyObject = other.m_PyObject;
    Py_XINCREF(m_PyObject);

    return *this;
  }

  template<typename T>
  inline T Object::ToValue()
  {
    PYB_ASSERT(IsValid());
    static std::string argumentString = BuildFunctionArgumentString<T>().c_str();
    std::tuple<ArgumentTypeHelper<T>::Type> tempValue;
    ArgumentHelper<ArgumentTypeHelper<T>::Type>::ParseValue(argumentString, m_PyObject, tempValue, gens<1>::type());
    T value = ArgumentTypeHelper<T>::Convert(std::get<0>(tempValue));
    return value;
  }

  template<typename ...ArgT>
  inline
    Object Object::Call(ArgT ...args)
  {
    PYB_ASSERT(IsCallable());

    Object arglist = BuildValueTuple<ArgT...>(args...);

    Object result = Object::FromNewRef(PyObject_Call(m_PyObject, arglist.m_PyObject, nullptr));

    return result;
  }
}
