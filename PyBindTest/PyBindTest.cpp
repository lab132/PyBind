// PyBindTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <PyBind.hpp>


int _tmain( int argc, wchar_t* argv [] )
{
  pyb::Interpreter interpreter;

  interpreter.Initialize( argv[ 0 ] );

  interpreter.RunString( "print(\"Hello\")" );


  pyb::Module module( "myTest", "some test module" );

  interpreter.RegisterModule( &module );

  interpreter.RunPyMain( argc, argv );

  interpreter.Finalize();
  return 0;
}

