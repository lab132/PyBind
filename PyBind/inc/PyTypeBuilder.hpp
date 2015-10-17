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
  };

  template<>
  struct PyTypeTrait<int>
  {
    static constexpr const char* PyTypeString = "i";
  };

  template<>
  struct PyTypeTrait<const char*>
  {
    static constexpr const char* PyTypeString = "s";
  };

  template<>
  struct PyTypeTrait<void>
  {
    static constexpr const char* PyTypeString = "";
  };

  template<>
  struct PyTypeTrait<std::string>
  {
    static constexpr const char* PyTypeString = "s";
  };

  template<typename T, typename RT, typename ...ArgT>
  struct BindDelegate
  {
    BaseBindObject<T> m_Object;

    RT(T::* m_Method)(ArgT...);
  };

  template<typename T = void>
  std::string BuildFunctionAgumentString()
  {
    return std::string( PyTypeTrait<T>::PyTypeString );
  }

  template<typename T, typename ...ArgT>
  std::string BuildFunctionAgumentString()
  {
    return std::string( PyTypeTrait<T>::PyTypeString ) + BuildFunctionAgumentString<ArgT...>();
  }

  template<typename T, typename RT = void, typename ...ArgT>
  struct BindMethodHelper
  {
    template< RT(T::*F)(ArgT... ) >
    static
    inline
    PyCFunction Bind( const char* name )
    {


      auto func = []( PyObject* self, PyObject* args )
      {
        BaseBindObject<T>* boundObject = reinterpret_cast< BaseBindObject<T>* >( self );

        ( boundObject->ptr->*F )( );
        Py_INCREF( Py_None );
        return Py_None;
      };

      return func;
    }


  };

  template<typename RT = void, typename ...ArgT>
  struct BindFunctionHelper
  {
    template< RT( *F )( ArgT... ) >
    static
      inline
      PyCFunction Bind( const char* name )
    {


      auto func = []( PyObject* self, PyObject* args )
      {

        ( *F )( );
        Py_INCREF( Py_None );
        return Py_None;
      };
      return func;
    }
  };

}
