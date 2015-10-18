#pragma once

#include "PyBindCommon.hpp"
#include <tuple>

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

  template<typename ... ArgT>
  struct ArgumentStringHelper;

  template<typename T,typename ... ArgT>
  struct ArgumentStringHelper<T, ArgT...>
  {
    static
    inline
    std::string BuildString()
    {
      return std::string( PyTypeTrait<T>::PyTypeString ) + ArgumentStringHelper<ArgT...>::BuildString();
    }


  };

  template<>
  struct ArgumentStringHelper<>
  {
    static
      inline
      std::string BuildString()
    {
      return std::string( "" );
    }


  };

  template<typename ...ArgT>
  std::string BuildFunctionArgumentString()
  {
    return ArgumentStringHelper<ArgT...>::BuildString();

  }

  template<int ...>
  struct seq
  {
  };

  template<int N, int ...S>
  struct gens : gens<N - 1, N - 1, S...>
  {
  };

  template<int ...S>
  struct gens<0, S...>
  {
    typedef seq<S...> type;
  };



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
  struct CallHelper
  {
    template< RT( *F )( ArgT... )>
    struct CallTypeHelper
    {
      template<size_t ...S>
      static
      inline
      void Call( const std::tuple<ArgT...>& arguments, seq<S...> )
      {
        ( *F )( std::get<S>( arguments )... );
      }

      template<size_t ...S>
      static
      inline
      void ParseArguments( const std::string& argumentString, PyObject* object, std::tuple<ArgT...>& arguments, seq<S...> )
      {
        int result = PyArg_ParseTuple( object, argumentString.c_str(), &std::get<S>( arguments )... );

        assert( result != 0 );
      }
    };
  };

  template<typename RT = void, typename ...ArgT>
  struct BindFunctionHelper
  {
    template< RT( *F )( ArgT... ) >
    static
      inline
      PyCFunction Bind()
    {


      PyCFunctionWithKeywords func = []( PyObject* self, PyObject* args, PyObject* keywords )
      {
        static std::string argumentString = BuildFunctionArgumentString<ArgT...>();

        std::tuple<ArgT...> arguments;
        CallHelper<RT, ArgT...>::CallTypeHelper<F>::ParseArguments( argumentString, args, arguments, gens<sizeof...( ArgT )>::type() );
        CallHelper<RT, ArgT...>::CallTypeHelper<F>::Call( arguments, gens<sizeof...( ArgT )>::type() );

        Py_INCREF( Py_None );
        return Py_None;
      };
      return reinterpret_cast< PyCFunction >( func );
    }
  };


  template<typename RT = void, typename ...ArgT>
  constexpr
  BindFunctionHelper<RT, ArgT...> Bind( RT( *F )( ArgT... ) )
  {
    return BindFunctionHelper<RT, ArgT...>();
  }



}
