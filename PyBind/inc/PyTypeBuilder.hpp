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
    static constexpr const char* PyVerboseString = "int";
  };

  template<>
  struct PyTypeTrait<const char*>
  {
    static constexpr const char* PyTypeString = "s";
    static constexpr const char* PyVerboseString = "string";
  };

  template<>
  struct PyTypeTrait<void>
  {
    static constexpr const char* PyTypeString = "";
    static constexpr const char* PyVerboseString = "void";
  };

  template<>
  struct PyTypeTrait<std::string>
  {
    static constexpr const char* PyTypeString = "s";
    static constexpr const char* PyVerboseString = "string";
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

    static
      inline
      std::string BuildVerboseString()
    {
      return std::string( PyTypeTrait<T>::PyVerboseString ) + ", " + ArgumentStringHelper<ArgT...>::BuildVerboseString();
    }


  };

  template<typename T>
  struct ArgumentStringHelper<T>
  {
    static
      inline
      std::string BuildString()
    {
      return std::string( PyTypeTrait<T>::PyTypeString );
    }

    static
      inline
      std::string BuildVerboseString()
    {
      return std::string( PyTypeTrait<T>::PyVerboseString );
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

    static
      inline
      std::string BuildVerboseString()
    {
      return std::string( "" );
    }


  };

  template<typename ...ArgT>
  std::string BuildFunctionArgumentString()
  {
    return ArgumentStringHelper<ArgT...>::BuildString();

  }

  template<typename ...ArgT>
  std::string BuildVerboseFunctionArgumentString()
  {
    return ArgumentStringHelper<ArgT...>::BuildVerboseString();

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

  template<typename ...>
  struct CallHelper
  {

  };

  // Call Helper for functions
  template<typename RT, typename ...ArgT>
  struct CallHelper<RT, ArgT...>
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
      bool ParseArguments( const std::string& argumentString, PyObject* object, std::tuple<ArgT...>& arguments, seq<S...> )
      {
        int result = PyArg_ParseTuple( object, argumentString.c_str(), &std::get<S>( arguments )... );

        if( result == 0 )
        {
          printf( "Could not parse arguments, expected: (%s)\n", BuildVerboseFunctionArgumentString<ArgT...>().c_str() );
          PyErr_Clear();

          return false;
        }
        return true;
      }
    };
  };

  // Call helper for methods
  template<typename T,typename RT, typename ...ArgT>
  struct CallHelper<RT( T::* )( ArgT... )>
  {
    template< RT( T::*F )( ArgT... )>
    struct CallTypeHelper
    {
      template<size_t ...S>
      static
        inline
        void Call( T* obj, const std::tuple<ArgT...>& arguments, seq<S...> )
      {
        ( obj->*F )( std::get<S>( arguments )... );
      }

      template<size_t ...S>
      static
        inline
        bool ParseArguments( const std::string& argumentString, PyObject* object, std::tuple<ArgT...>& arguments, seq<S...> )
      {
        int result = PyArg_ParseTuple( object, argumentString.c_str(), &std::get<S>( arguments )... );

        if( result == 0 )
        {
          printf( "Could not parse arguments, expected: (%s)\n", BuildVerboseFunctionArgumentString<ArgT...>().c_str() );
          PyErr_Clear();
          return false;
        }
        return true;
      }
    };
  };

  template <typename ...>
  struct BindFunctionHelper
  {

  };

  template<typename RT, typename ...ArgT>
  struct BindFunctionHelper<RT( ArgT... )>
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
        if( CallHelper<RT, ArgT...>::CallTypeHelper<F>::ParseArguments( argumentString, args, arguments, gens<sizeof...( ArgT )>::type() ) )
        {
          CallHelper<RT, ArgT...>::CallTypeHelper<F>::Call( arguments, gens<sizeof...( ArgT )>::type() );
        }

        Py_INCREF( Py_None );
        return Py_None;
      };
      return reinterpret_cast< PyCFunction >( func );
    }
  };


  template<typename RT, typename ...ArgT>
  constexpr
  BindFunctionHelper<RT(ArgT...)> Bind( RT( *F )( ArgT... ) )
  {
    return BindFunctionHelper<RT( ArgT...)>();
  }


  template<typename T, typename RT, typename ...ArgT>
  struct BindFunctionHelper<RT( T::* )( ArgT...)>
  {
    template< RT( T::*F )( ArgT... ) >
    static
      inline
      PyCFunction Bind()
    {


      PyCFunctionWithKeywords func = []( PyObject* self, PyObject* args, PyObject* keywords )
      {
        static std::string argumentString = BuildFunctionArgumentString<ArgT...>();

        BaseBindObject<T>* typedSelf = reinterpret_cast< BaseBindObject<T>* >( self );

        std::tuple<ArgT...> arguments;
        if( CallHelper<RT( T::* )( ArgT... )>::CallTypeHelper<F>::ParseArguments( argumentString, args, arguments, gens<sizeof...( ArgT )>::type() ) )
        {
          CallHelper<RT( T::* )( ArgT... )>::CallTypeHelper<F>::Call( typedSelf->ptr, arguments, gens<sizeof...( ArgT )>::type() );
        }

        Py_INCREF( Py_None );
        return Py_None;
      };
      return reinterpret_cast< PyCFunction >( func );
    }


  };

  template<typename T, typename RT, typename ...ArgT>
  constexpr
    BindFunctionHelper<RT(T::*)(ArgT...)> Bind( RT( T::*F )( ArgT... ) )
  {
    return BindFunctionHelper<RT(T::*)(ArgT...)>();
  }




}
