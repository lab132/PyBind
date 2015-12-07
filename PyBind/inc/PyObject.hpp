#pragma once

#include "PyBindCommon.hpp"

namespace pyb
{
  class BaseTypeObject;
  class Dictionary;
  class List;

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

    bool IsList() const;

    bool IsCallable() const;

    void Invalidate();

    bool IsNone() const;

    bool IsOfType(BaseTypeObject* type) const;

    /**
    @brief Increments the ref counter of the underlying Python Objects.
    THIS CAN LEAD TO MEMORY LEAKS if used without reasons (usually, when directly working with the python api)

    This is usually used when a API call steals a reference to an object.
    */
    void IncrementRefCount();
    /**
    @brief Increments the ref counter of the underlying Python Objects.
    THIS CAN DESTROY REFERENCE TOO EARLY if used without reasons (usually, when directly working with the python api)

    This is usually used when a API call steals a reference to an object.
    */
    void DecrementRefCount();

    template<typename T>
    T ToValue();

    Dictionary ToDictionary();
    List ToList();

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
