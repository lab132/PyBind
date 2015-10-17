#pragma once

#include "PyBindCommon.hpp"

namespace pyb
{
  template <typename T>
  struct BaseBindObject
  {
    PyObject_HEAD
    T* ptr;
  };

  template<typename T>
  struct PyTypeTrait
  {
    static_assert(false, "Could not deduce type");
  };

  template<int>
  struct PyTypeTrait
  {
    const char* PyTypeString = "i";
  };

  template<const char*>
  struct PyTypeTrait
  {
    const char* PyTypeString = "s";
  };

  template<std::string>
  struct PyTypeTrait
  {
    const char* PyTypeString = "s";
  };

  template<typename T, typename RT, typename ...ArgT>
  struct BindDelegate
  {
    BaseBindObject<T> m_Object;

    RT(T::* m_Method)(...ArgT);
  };

  template<typename T, typename RT, typename ...ArgT>
  BindDelegate<T, RT, ArgT...> BuildDelegate(T* pointer, RT(T::* method)(ArgT...)
  {
    BindDelegate<T, RT, ArgT...> delegate;
    delegate.m_Object.ptr = pointer;
  }
}
