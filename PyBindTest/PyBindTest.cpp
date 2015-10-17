// PyBindTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <PyBind.hpp>

struct Test
{
  void test()
  {
    printf( "YAY!!!" );
  }
};

void test()
{
  printf( "YAY!!! FUNCTION!!!\n" );
}

int _tmain( int argc, wchar_t* argv [] )
{
  pyb::Interpreter interpreter;

  interpreter.Initialize( argv[ 0 ] );

  interpreter.RunString( "print(\"Hello\")" );


  pyb::Module module( "myTest", "some test module" );

  interpreter.RegisterModule( &module );

  module.AddFunction( pyb::BindFunctionHelper<>::Bind<&test>( "test" ), "test" );

  interpreter.RunPyMain( argc, argv );

  Test t;

  pyb::BindMethodHelper<Test>::Bind<&Test::test>("test");


  interpreter.Finalize();
  return 0;
}

