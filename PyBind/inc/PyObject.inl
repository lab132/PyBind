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
    T value;
    PyArg_Parse(m_PyObject, BuildFunctionArgumentString<T>().c_str(), &value);
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
