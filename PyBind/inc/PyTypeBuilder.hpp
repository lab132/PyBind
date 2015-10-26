#pragma once

#include "PyBindCommon.hpp"
#include <tuple>

namespace pyb
{
  struct BaseBindObject
  {
    PyObject_HEAD
    void* ptr;
  };

  struct BindDelegate
  {
    bool ClassBinding;
    PyCFunction Function;
    const char* Name;
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
  struct PyTypeTrait<unsigned char>
  {
    static constexpr const char* PyTypeString = "b";
    static constexpr const char* PyVerboseString = "unsigned char";
  };

  template<>
  struct PyTypeTrait<char>
  {
    static constexpr const char* PyTypeString = "B";
    static constexpr const char* PyVerboseString = "char";
  };

  template<>
  struct PyTypeTrait<unsigned short>
  {
    static constexpr const char* PyTypeString = "h";
    static constexpr const char* PyVerboseString = "unsigned short";
  };

  template<>
  struct PyTypeTrait<short>
  {
    static constexpr const char* PyTypeString = "H";
    static constexpr const char* PyVerboseString = "short";
  };

  template<>
  struct PyTypeTrait<unsigned int>
  {
    static constexpr const char* PyTypeString = "I";
    static constexpr const char* PyVerboseString = "unsigned int";
  };

  template<>
  struct PyTypeTrait<unsigned long long>
  {
    static constexpr const char* PyTypeString = "K";
    static constexpr const char* PyVerboseString = "unsigned long long";
  };

  template<>
  struct PyTypeTrait<long long>
  {
    static constexpr const char* PyTypeString = "L";
    static constexpr const char* PyVerboseString = "long long";
  };

  template<>
  struct PyTypeTrait<bool>
  {
    static constexpr const char* PyTypeString = "p";
    static constexpr const char* PyVerboseString = "bool";
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
  struct PyTypeTrait<float>
  {
    static constexpr const char* PyTypeString = "f";
    static constexpr const char* PyVerboseString = "float";
  };

  template<>
  struct PyTypeTrait<double>
  {
    static constexpr const char* PyTypeString = "d";
    static constexpr const char* PyVerboseString = "double";
  };

  template<>
  struct PyTypeTrait<std::string>
  {
    static constexpr const char* PyTypeString = "s";
    static constexpr const char* PyVerboseString = "string";
  };

  template<typename T>
  struct PyTypeTrait<T*>
  {
    static constexpr const char* PyTypeString = "o";
    static constexpr const char* PyVerboseString = "pointer";
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

  // Call Helper for functions
  template<typename ...ArgT>
  struct ArgumentHelper
  {
    template<size_t ...S>
    static
      inline
      bool ParseArguments(const std::string& argumentString, PyObject* object, std::tuple<ArgT...>& arguments, seq<S...>)
    {
      int result = PyArg_ParseTuple(object, argumentString.c_str(), &std::get<S>(arguments)...);

      if(result == 0)
      {
        printf("Could not parse arguments, expected: (%s)\n", BuildVerboseFunctionArgumentString<ArgT...>().c_str());
        PyErr_Clear();
        return false;
      }
      return true;
    }
  };


  // Call helper for constructors
  template<typename T, typename ...ArgT>
  struct CtorCallHelper
  {
    template<size_t ...S>
    static
      inline
      void Call(T* obj, const std::tuple<ArgT...>& arguments, seq<S...>)
    {
      new (obj) T( std::get<S>(arguments)... );
    }
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
      RT Call( const std::tuple<ArgT...>& arguments, seq<S...> )
      {
        return ( *F )( std::get<S>( arguments )... );
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
        RT Call( T* obj, const std::tuple<ArgT...>& arguments, seq<S...> )
      {
        return ( obj->*F )( std::get<S>( arguments )... );
      }
    };
  };

  // Call helper for const methods
  template<typename T, typename RT, typename ...ArgT>
  struct CallHelper<RT( T::* )( ArgT... ) const>
  {
    template< RT( T::*F )( ArgT... ) const>
    struct CallTypeHelper
    {
      template<size_t ...S>
      static
        inline
        RT Call( T* obj, const std::tuple<ArgT...>& arguments, seq<S...> )
      {
        return ( obj->*F )( std::get<S>( arguments )... );
      }
    };
  };

  template <typename ...>
  struct BindHelper
  {

  };

  // Bind helper for functions
  template<typename RT, typename ...ArgT>
  struct BindHelper<RT( ArgT... )>
  {
    template< RT( *F )( ArgT... ) >
    static
      inline
      BindDelegate Bind( const char* name)
    {


      PyCFunctionWithKeywords func = []( PyObject* self, PyObject* args, PyObject* keywords )
      {
        static std::string argumentString = BuildFunctionArgumentString<ArgT...>();

        std::tuple<ArgT...> arguments;


        if( CallHelper<RT, ArgT...>::CallTypeHelper<F>::ParseArguments( argumentString, args, arguments, gens<sizeof...( ArgT )>::type() ) )
        {
          RT result;
          result = CallHelper<RT, ArgT...>::CallTypeHelper<F>::Call( arguments, gens<sizeof...( ArgT )>::type() );

          PyObject* obj = Py_BuildValue(PyTypeTrait<RT>::PyTypeString, result);
          return obj;
        }
        else
        {
          Py_INCREF(Py_None);
          return Py_None;
        }

      };
      return BindDelegate{false, reinterpret_cast<PyCFunction>(func), name};
    }
  };

  // Bind helper for functions
  template<typename ...ArgT>
  struct BindHelper<void (ArgT...)>
  {
    template< void(*F)(ArgT...) >
    static
      inline
      BindDelegate Bind(const char* name)
    {


      PyCFunctionWithKeywords func = [](PyObject* self, PyObject* args, PyObject* keywords)
      {
        static std::string argumentString = BuildFunctionArgumentString<ArgT...>();

        std::tuple<ArgT...> arguments;


        if(ArgumentHelper<ArgT...>::ParseArguments(argumentString, args, arguments, gens<sizeof...(ArgT)>::type()))
        {
           CallHelper<void, ArgT...>::CallTypeHelper<F>::Call(arguments, gens<sizeof...(ArgT)>::type());
        }

        Py_INCREF(Py_None);
        return Py_None;
      };
      return BindDelegate{false, reinterpret_cast<PyCFunction>(func), name};
    }
  };

  // Bind helper for methods
  template<typename T, typename RT, typename ...ArgT>
  struct BindHelper<RT( T::* )( ArgT...)>
  {
    template< RT( T::*F )( ArgT... ) >
    static
      inline
      BindDelegate Bind( const char* name)
    {

      PyCFunctionWithKeywords func = []( PyObject* self, PyObject* args, PyObject* keywords )
      {
        static std::string argumentString = BuildFunctionArgumentString<ArgT...>();

        BaseBindObject* typedSelf = reinterpret_cast< BaseBindObject* >( self );

        std::tuple<ArgT...> arguments;
        if(ArgumentHelper<ArgT...>::ParseArguments(argumentString, args, arguments, gens<sizeof...(ArgT)>::type()))
        {
          RT result;
          result = CallHelper<RT(T::*)(ArgT...)>::CallTypeHelper<F>::Call(reinterpret_cast<T*>(typedSelf->ptr), arguments, gens<sizeof...(ArgT)>::type());

          PyObject* obj = Py_BuildValue(PyTypeTrait<RT>::PyTypeString, result);
          return obj;
        }
        else
        {
          Py_INCREF(Py_None);
          return Py_None;
        }
      };
      return BindDelegate{true, reinterpret_cast<PyCFunction>(func), name};
    }


  };

  // Bind helper for const methods
  template<typename T, typename RT, typename ...ArgT>
  struct BindHelper<RT( T::* )( ArgT... ) const>
  {
    template< RT( T::*F )( ArgT... ) const>
    static
      inline
      BindDelegate Bind(const char* name)
    {

      PyCFunctionWithKeywords func = []( PyObject* self, PyObject* args, PyObject* keywords )
      {
        static std::string argumentString = BuildFunctionArgumentString<ArgT...>();

        BaseBindObject* typedSelf = reinterpret_cast< BaseBindObject* >( self );

        std::tuple<ArgT...> arguments;
        if(ArgumentHelper<ArgT...>::ParseArguments(argumentString, args, arguments, gens<sizeof...(ArgT)>::type()))
        {
          RT result;
          result = CallHelper<RT (T::*)(ArgT...) const>::CallTypeHelper<F>::Call(reinterpret_cast<T*>(typedSelf->ptr), arguments, gens<sizeof...(ArgT)>::type());

          PyObject* obj = Py_BuildValue(PyTypeTrait<RT>::PyTypeString, result);
          return obj;
        }
        else
        {
          Py_INCREF(Py_None);
          return Py_None;
        }
      };
      return BindDelegate{true, reinterpret_cast<PyCFunction>(func), name};
    }


  };

  // Bind helper for const methods
  template<typename T, typename ...ArgT>
  struct BindHelper<void(T::*)(ArgT...) const>
  {
    template< void(T::*F)(ArgT...) const>
    static
      inline
      BindDelegate Bind(const char* name)
    {

      PyCFunctionWithKeywords func = [](PyObject* self, PyObject* args, PyObject* keywords)
      {
        static std::string argumentString = BuildFunctionArgumentString<ArgT...>();

        BaseBindObject* typedSelf = reinterpret_cast<BaseBindObject*>(self);
        std::tuple<ArgT...> arguments;
        if(ArgumentHelper<ArgT...>::ParseArguments(argumentString, args, arguments, gens<sizeof...(ArgT)>::type()))
        {
          CallHelper<void(T::*)(ArgT...) const>::CallTypeHelper<F>::Call(reinterpret_cast<T*>(typedSelf->ptr), arguments, gens<sizeof...(ArgT)>::type());
        }

        Py_INCREF(Py_None);
        return Py_None;

      };
      return BindDelegate{true, reinterpret_cast<PyCFunction>(func), name};
    }


  };

  // Bind helper for const methods
  template<typename T, typename ...ArgT>
  struct BindHelper<void(T::*)(ArgT...)>
  {
    template< void(T::*F)(ArgT...)>
    static
      inline
      BindDelegate Bind(const char* name)
    {

      PyCFunctionWithKeywords func = [](PyObject* self, PyObject* args, PyObject* keywords)
      {
        static std::string argumentString = BuildFunctionArgumentString<ArgT...>();

        BaseBindObject* typedSelf = reinterpret_cast<BaseBindObject*>(self);
        std::tuple<ArgT...> arguments;
        if(ArgumentHelper<ArgT...>::ParseArguments(argumentString, args, arguments, gens<sizeof...(ArgT)>::type()))
        {
          CallHelper<void(T::*)(ArgT...)>::CallTypeHelper<F>::Call( reinterpret_cast<T*>( typedSelf->ptr ), arguments, gens<sizeof...(ArgT)>::type());
        }

        Py_INCREF(Py_None);
        return Py_None;

      };
      return BindDelegate{true, reinterpret_cast<PyCFunction>(func), name};
    }


  };

  // Bind helper for const methods
  template<typename T>
  struct CtorHelper
  {
    template<typename ...ArgT>
    static
      inline
      BindDelegate Bind()
    {

      initproc func = [](PyObject* self, PyObject* args, PyObject* keywords)
      {
        static std::string argumenString = BuildFunctionArgumentString<ArgT...>();

        BaseBindObject* newObj = reinterpret_cast<BaseBindObject*>(self);

        std::tuple<ArgT...> arguments;

        if(ArgumentHelper<ArgT...>::ParseArguments(argumenString, args, arguments, gens<sizeof...(ArgT)>::type()))
        {
          CtorCallHelper<T, ArgT...>::Call(static_cast<T*>( newObj->ptr ), arguments, gens<sizeof...(ArgT)>::type());
          return 0;
        }
        else
        {
          return -1;
        }

      };
      return BindDelegate{true, reinterpret_cast<PyCFunction>(func), nullptr};
    }


  };

  template<typename RT, typename ...ArgT>
  constexpr
    BindHelper<RT( ArgT... )> Bind( RT( *F )( ArgT... ) )
  {
    return BindHelper<RT( ArgT... )>();
  }

  template<typename T, typename RT, typename ...ArgT>
  constexpr
    BindHelper<RT( T::* )( ArgT... )> Bind( RT( T::*F )( ArgT... ) )
  {
    return BindHelper<RT( T::* )( ArgT... )>();
  }

  template<typename T, typename RT, typename ...ArgT>
  constexpr
    BindHelper<RT( T::* )( ArgT... ) const> Bind( RT( T::*F )( ArgT... ) const )
  {
    return BindHelper<RT( T::* )( ArgT... ) const>();
  }

}
