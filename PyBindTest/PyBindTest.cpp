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

void test(int a, int b, const char* string)
{
  printf( "Got %d, %d and %s\n", a,b,string );
}

int _tmain( int argc, wchar_t* argv [] )
{
  pyb::Interpreter interpreter;

  interpreter.Initialize( argv[ 0 ] );

  interpreter.RunString( "print(\"Hello\")" );


  pyb::Module module( "myTest", "some test module" );

  interpreter.RegisterModule( &module );

  module.AddFunction( pyb::BindFunctionHelper<void,int,int,const char*>::Bind<&test>( "t" ), "t" );

  interpreter.RunPyMain( argc, argv );

  //Test t;

  //pyb::BindMethodHelper<Test>::Bind<&Test::test>("test");


  interpreter.Finalize();
  return 0;
}

