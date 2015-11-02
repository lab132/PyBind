#pragma once

#include "PyBindCommon.hpp"
#include "PyObject.hpp"
#include "PyModule.hpp"
#include <stdio.h>
#include <vector>
#include <xutility>

namespace pyb
{
  class Module;
  class Interpreter
  {
  public:
    Interpreter();

    /**
    @brief Initializes the interpreter with the given programName.
    The interpreter is usable after this call.
    */
    void Initialize( wchar_t* programName );

    /**
    @brief Destroys the interpreter, freeing all resources.
    After this point the interpreter and all python related stuff should not be used anymore.

    */
    void Finalize();

    /**
    @brief Runs a python interpreter with the given arguments.
    The args behave the same as passed into python.exe directly.

    If called with no arguments this method will block execution and display an user interactive command line
    in the console.

    @param argc number of arguments
    @param argv the arguments, encoded in utf-16
    */
    void RunPyMain( int argc, wchar_t* argv [] );

    /**

    @param globals pass a custom globals dict, if nullptr is used it will be replaced with the __main__ module dict
    @param locals pass a custom locals dict, if nullptr is used it will be replaced with the __main__ module dict
    @return the result of the python expression
    */
    Object RunString( const std::string& expression, const Object* globals = nullptr, const Object* locals = nullptr );

    /**

    @param globals pass a custom globals dict, if nullptr is used it will be replaced with the __main__ module dict
    @param locals pass a custom locals dict, if nullptr is used it will be replaced with the __main__ module dict
    @return the result of the python file
    */
    Object RunFile( const std::string& fileName, const Object* globals = nullptr, const Object* locals = nullptr );

    /**
    @brief Retrieves the module dictionary from the __main__ module in python.
    The returned pointer is owned by the Interpreter and is valid as long as the interpreter exists.

    @return A pointer to the owned module dictionary
    */
    const Object& GetMainDict();

    void RegisterModule( Module* module );

  private:
    Object m_GlobalsDict;

    std::vector< Module* > m_Modules;
  };

}
