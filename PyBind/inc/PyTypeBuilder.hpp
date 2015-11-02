#pragma once

#include "PyBindCommon.hpp"
#include "PyTypeTraits.hpp"
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


  template<typename ... ArgT>
  Object BuildValue(ArgT... arguments)
  {
    static std::string argumentString = BuildFunctionArgumentString<ArgT...>();

    return Object::FromNewRef(Py_BuildValue(argumentString.c_str(), arguments...));
  }

  template<typename ... ArgT>
  Object BuildValueTuple(ArgT... arguments)
  {
    static std::string argumentString = "(" + BuildFunctionArgumentString<ArgT...>() + ")";

    return Object::FromNewRef(Py_BuildValue(argumentString.c_str(), arguments...));
  }

  template<typename T>
  struct ArgumentTypeHelper
  {
    typedef T Type;
    typedef T WrappedType;

    constexpr
    static
    inline
    WrappedType Convert(Type t)
    {
      return t;
    }
  };

  template<>
  struct ArgumentTypeHelper<const char*>
  {
    typedef const char* Type;
    typedef const char* WrappedType;

    static
      inline
      WrappedType Convert(Type t)
    {
      return t;
    }
  };

  template<typename T>
  struct ArgumentTypeHelper<T*>
  {
    typedef BaseBindObject* Type;
    typedef T* WrappedType;

    static
    inline
    WrappedType Convert(Type t)
    {
      return reinterpret_cast<WrappedType>(t->ptr);
    }
  };

  template<typename A, typename B>
  void Set(A& from, B& to)
  {
    to = ArgumentTypeHelper<B>::Convert(from);
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

  template<typename ...Arg1>
  struct TupleConvertHelper
  {
    template<typename ...Arg2>
    struct TupleHelper
    {
      template<size_t ...S>
      inline
        static
        void ConvertTo(std::tuple<Arg1...>& from, std::tuple<Arg2...>& to, seq<S...>)
      {
        using expander = int [];
        expander{0,
          (void(Set(std::get<S>(from),std::get<S>(to))), 0)...
        };
      }
    };
  };

  // Call Helper for functions
  template<typename ...ArgT>
  struct ArgumentHelper
  {
    template<size_t ...S>
    static
      inline
      bool ParseArguments(const std::string& argumentString, PyObject* object, std::tuple<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ... >& arguments, seq<S...>)
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


        std::tuple<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ...> arguments;
        std::tuple<ArgT...> finalArguments;

        if( ArgumentHelper<ArgT...>::ParseArguments( argumentString, args, arguments, gens<sizeof...( ArgT )>::type() ) )
        {
          TupleConvertHelper<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ... >::TupleHelper<ArgT...>::ConvertTo(
            arguments, finalArguments, gens<sizeof...(ArgT)>::type());

          RT result;
          result = CallHelper<RT, ArgT...>::CallTypeHelper<F>::Call( finalArguments, gens<sizeof...( ArgT )>::type() );

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

        std::tuple<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ...> arguments;
        std::tuple<ArgT...> finalArguments;

        if(ArgumentHelper<ArgT...>::ParseArguments(argumentString, args, arguments, gens<sizeof...(ArgT)>::type()))
        {
          TupleConvertHelper<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ... >::TupleHelper<ArgT...>::ConvertTo(
            arguments, finalArguments, gens<sizeof...(ArgT)>::type());
          CallHelper<void, ArgT...>::CallTypeHelper<F>::Call(finalArguments, gens<sizeof...(ArgT)>::type());
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


        std::tuple<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ...> arguments;
        std::tuple<ArgT...> finalArguments;
        if(ArgumentHelper<ArgT...>::ParseArguments(argumentString, args, arguments, gens<sizeof...(ArgT)>::type()))
        {
          TupleConvertHelper<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ... >::TupleHelper<ArgT...>::ConvertTo(
            arguments, finalArguments, gens<sizeof...(ArgT)>::type());
          RT result;
          result = CallHelper<RT(T::*)(ArgT...)>::CallTypeHelper<F>::Call(reinterpret_cast<T*>(typedSelf->ptr), finalArguments, gens<sizeof...(ArgT)>::type());

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

        std::tuple<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ...> arguments;
        std::tuple<ArgT...> finalArguments;
        if(ArgumentHelper<ArgT...>::ParseArguments(argumentString, args, arguments, gens<sizeof...(ArgT)>::type()))
        {
          TupleConvertHelper<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ... >::TupleHelper<ArgT...>::ConvertTo(
            arguments, finalArguments, gens<sizeof...(ArgT)>::type());
          RT result;
          result = CallHelper<RT (T::*)(ArgT...) const>::CallTypeHelper<F>::Call(
            reinterpret_cast<T*>(typedSelf->ptr), finalArguments, gens<sizeof...(ArgT)>::type());

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

        std::tuple<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ...> arguments;
        std::tuple<ArgT...> finalArguments;
        if(ArgumentHelper<ArgT...>::ParseArguments(argumentString, args, arguments, gens<sizeof...(ArgT)>::type()))
        {
          TupleConvertHelper<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ... >::TupleHelper<ArgT...>::ConvertTo(
            arguments, finalArguments, gens<sizeof...(ArgT)>::type());

          CallHelper<void(T::*)(ArgT...) const>::CallTypeHelper<F>::Call(
            reinterpret_cast<T*>(typedSelf->ptr), finalArguments, gens<sizeof...(ArgT)>::type());
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

        std::tuple<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ...> arguments;
        std::tuple<ArgT...> finalArguments;

        if(ArgumentHelper<ArgT...>::ParseArguments(argumentString, args, arguments, gens<sizeof...(ArgT)>::type()))
        {
          TupleConvertHelper<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ... >::TupleHelper<ArgT...>::ConvertTo(
            arguments, finalArguments, gens<sizeof...(ArgT)>::type());

          CallHelper<void(T::*)(ArgT...)>::CallTypeHelper<F>::Call(
            reinterpret_cast<T*>( typedSelf->ptr ), finalArguments, gens<sizeof...(ArgT)>::type());
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

        std::tuple<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ...> arguments;
        std::tuple<ArgT...> finalArguments;

        if(ArgumentHelper<ArgT...>::ParseArguments(argumenString, args, arguments, gens<sizeof...(ArgT)>::type()))
        {
          TupleConvertHelper<decltype((ArgumentTypeHelper<ArgT>::Type)nullptr) ... >::TupleHelper<ArgT...>::ConvertTo(
            arguments, finalArguments, gens<sizeof...(ArgT)>::type());

          CtorCallHelper<T, ArgT...>::Call(static_cast<T*>( newObj->ptr ), finalArguments, gens<sizeof...(ArgT)>::type());
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
