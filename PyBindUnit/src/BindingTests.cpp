#include "catch.hpp"

#include "PyBindUnit.hpp"
#include <string>

bool calledFunction = false;

void TestFunction1()
{
  calledFunction = true;
}

struct Function2Result
{
  int i;
  unsigned char b;
  char B;
  short h;
  unsigned short H;
  unsigned int I;
  unsigned long long K;
  long long L;
  bool p;
  std::string s;
  float f;
  double d;
  std::string str;
};

Function2Result f2Result;

void TestFunction2(
  int i
  ,unsigned char b
  ,char B
  ,unsigned short H
  ,short h
  ,unsigned int I
  ,unsigned long long K
  ,long long L
  ,bool p
  ,const char* s
  ,float f
  ,double d
  ,std::string str)
{
  f2Result.i = i;
  f2Result.b = b;
  f2Result.B = B;
  f2Result.H = H;
  f2Result.h = h;
  f2Result.I = I;
  f2Result.K = K;
  f2Result.L = L;
  f2Result.p = p;
  f2Result.s = s;
  f2Result.f = f;
  f2Result.d = d;
  f2Result.str = str;
  calledFunction = true;
}

int TestFunction3(int a, int b)
{
  calledFunction = true;
  return a + b;
}

SCENARIO("Module bind Test", "[binding][module]")
{
  GIVEN("An interpreter")
  {
    Interpreter interpreter;
    interpreter.Initialize();

    WHEN("Binding a module")
    {
      Module module = Module("testModule");

      interpreter.RegisterModule(&module);

      THEN("It should be importable")
      {
        auto result = interpreter.RunString("import testModule");

        REQUIRE(result.IsValid());
      }
    }

    interpreter.Finalize();
  }
}

SCENARIO("Function bind Test", "[binding][function]")
{
  GIVEN("An interpreter with a bound module")
  {
    Interpreter interpreter;
    interpreter.Initialize();
    {
      Module module = Module("testModule");

      interpreter.RegisterModule(&module);
      interpreter.RunString("import testModule");

      calledFunction = false;

      WHEN("Binding a function to the module")
      {
        REQUIRE(calledFunction == false);
        module.AddFunction(PY_BIND_FUNCTION(TestFunction1));
        REQUIRE(calledFunction == false);

        THEN("It should be callable")
        {

          auto result = interpreter.RunString("testModule.TestFunction1()");
          REQUIRE(calledFunction == true);
          REQUIRE(result.IsValid());
        }
      }

      WHEN("Binding a function to the module with a custom name")
      {
        REQUIRE(calledFunction == false);
        module.AddFunction(PY_BIND_FUNCTION_NAMED(TestFunction1, "myNamedFunction"));
        REQUIRE(calledFunction == false);

        THEN("It should be callable")
        {

          auto result = interpreter.RunString("testModule.myNamedFunction()");
          REQUIRE(calledFunction == true);
          REQUIRE(result.IsValid());
        }
      }

      WHEN("Binding a function with parameters")
      {
        REQUIRE(calledFunction == false);
        module.AddFunction(PY_BIND_FUNCTION(TestFunction2));
        REQUIRE(calledFunction == false);

        THEN("The parameters should be passed correctly")
        {

          auto result = interpreter.RunString("testModule.TestFunction2("
            "-2147483647"
            ",255"
            ",-127"
            ",65535"
            ",-32768"
            ",4294967295"
            ",18446744073709551615"
            ",-9223372036854775808"
            ",True"
            ",'hello'"
            ",3.4E+38"
            ",1.7E-308"
            ",'hello again'"
            ")");
          REQUIRE(calledFunction == true);
          REQUIRE(result.IsValid());

          REQUIRE(f2Result.i == -2147483647);
          REQUIRE(f2Result.b == 255);
          REQUIRE(f2Result.B == -127);
          REQUIRE(f2Result.H == 65535);
          REQUIRE(f2Result.h == -32768);
          REQUIRE(f2Result.I == 4294967295U);
          REQUIRE(f2Result.K == 18446744073709551615ULL);
          REQUIRE(f2Result.L == -9223372036854775808LL);
          REQUIRE(f2Result.p == true);
          REQUIRE(f2Result.s == "hello");
          REQUIRE(f2Result.f == 3.4E+38f);
          REQUIRE(f2Result.d == 1.7E-308);
          REQUIRE(f2Result.str == "hello again");
        }
      }

      WHEN("Binding a function with a return value")
      {
        REQUIRE(calledFunction == false);
        module.AddFunction(PY_BIND_FUNCTION(TestFunction3));
        REQUIRE(calledFunction == false);
        THEN("It should return the expected value")
        {
          auto result = interpreter.RunString("f=testModule.TestFunction3(3,5)");
          REQUIRE(calledFunction);
          REQUIRE(result.IsValid());
          auto mainDictObj = interpreter.GetMainDict();
          REQUIRE(mainDictObj.IsValid());
          REQUIRE(mainDictObj.IsDictionary());
          Dictionary dict = Dictionary::FromObject(mainDictObj);
          int resultInt = dict.GetItem<int>("f");
          REQUIRE(resultInt == 8);
        }
      }
    }

    interpreter.Finalize();
  }
}
