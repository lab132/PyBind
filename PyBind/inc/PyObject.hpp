#pragma once

#include "PyBindCommon.hpp"

namespace pyb
{
  class Object
  {
  public:
    Object( const Object& obj );
    Object( Object&& other );
    Object();
    ~Object();

    Object& operator =( const Object& other );

    PyObject* ObjectPtr() const;

    PyObject* m_PyObject;

    /**
    @brief Checks whether the wrapped PythonObject is a valid pointer or not
    */
    bool IsValid() const;

    bool IsDictionary() const;

    bool IsCallable() const;

    /*template<typename RT, typename ...ArgT>
    RT Call(ArgT... args);*/

    /**
    @brief Creates new Object and owns it (incrementing the refCount)
    This should be used if a python API function returns a borrowed reference

    @param pyObject the reference to be owned
    @return The created wrapper, owning the python object
    */
    static Object FromBorrowed( PyObject* pyObject );

    /**
    @brief Creates new Object wrapper for a newly created python object
    This should be used if a python API function returns a new reference

    @param pyObject the reference to be new python Object
    @return The created wrapper, for the python object
    */
    static Object FromNewRef( PyObject* pyObject );
  };

  inline
    Object::Object( const Object & obj ) :
    m_PyObject( obj.m_PyObject )
  {
    Py_XINCREF( m_PyObject );
  }

  inline
    Object::Object( Object && other ) :
    m_PyObject( other.m_PyObject )
  {
    other.m_PyObject = nullptr;
  }

  inline Object::Object() :
    m_PyObject( nullptr )
  {
  }

  inline
    Object::~Object()
  {
    Py_XDECREF( m_PyObject );
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
    return PyDict_Check( m_PyObject );
  }

  inline
  bool Object::IsCallable() const
  {
    return PyCallable_Check(m_PyObject);
  }

  inline
    Object Object::FromBorrowed( PyObject * pyObject )
  {
    Object obj;
    obj.m_PyObject = pyObject;
    Py_XINCREF( pyObject );
    return obj;
  }

  inline
    Object Object::FromNewRef( PyObject* pyObject )
  {
    Object obj;
    obj.m_PyObject = pyObject;
    return obj;
  }

  inline
    Object & pyb::Object::operator=( const Object & other )
  {
    m_PyObject = other.m_PyObject;
    Py_XINCREF( m_PyObject );

    return *this;
  }
  /*template<typename ...ArgT>
  inline
  Object Object::Call(ArgT ...args)
  {
    static std::string argumentString = "(" + BuildFunctionArgumentString<ArgT...>() + ")";

    Object arglist = BuildValue()
  }*/
}
