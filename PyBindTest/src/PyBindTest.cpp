// PyBindTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <PyBind.hpp>

struct Test
{
  int a;

  Test(int a, int b);

  void SetA(int value)
  {
    a = value;
  }
  int GetA() const
  {
    return a;
  }
};

Test::Test(int a, int b)
{
  this->a = a;
}


int test2( int a, int b)
{
  printf( "Got 2 %d, %d\n", a, b);
  return a + b;
}

void test(int a, int b, Test* t)
{
  printf( "Got %d, %d and %d\n", a,b,t->a);
}

int _tmain( int argc, wchar_t* argv [] )
{
  pyb::Interpreter interpreter;

  interpreter.Initialize( argv[ 0 ] );

  interpreter.RunString("f = lambda n: print('Got {0}'.format(n))");

  auto obj = (interpreter.GetMainDict());

  bool isDict = obj.IsDictionary();

  pyb::Dictionary dict = pyb::Dictionary::FromObject(obj);

  auto callable = dict.GetItem("f");

  callable.Call(1);

  pyb::Module module( "myTest", "some test module" );

  interpreter.RegisterModule( &module );

  module.AddFunction( PY_BIND_FUNCTION( test ) );
  module.AddFunction( PY_BIND_FUNCTION( test2 ));

  pyb::TypeObject<Test> classDef = pyb::TypeObject<Test>("testobj");
  classDef.AddProperty(PY_BIND_PROPERTY(Test, A));
  classDef.SetConstructor( PY_BIND_CTOR( Test, int, int ) );

  module.AddType(&classDef);

  interpreter.RunPyMain( argc, argv );

  //Test t;

  //pyb::BindMethodHelper<Test>::Bind<&Test::test>("test");




  interpreter.Finalize();
  return 0;
}

