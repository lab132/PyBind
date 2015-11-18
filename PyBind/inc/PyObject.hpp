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

    void Invalidate();

    bool IsNone() const;

    template<typename T>
    T ToValue();

    template<typename ...ArgT>
    Object Call(ArgT... args);

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


}
