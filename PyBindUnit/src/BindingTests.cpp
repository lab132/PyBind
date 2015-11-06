#include "catch.hpp"

#include "PyBindUnit.hpp"

bool calledFunction = false;

void TestFunction1()
{
  calledFunction = true;
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

      calledFunction = false;

      WHEN("Binding a function to the module")
      {
        REQUIRE(calledFunction == false);
        module.AddFunction(PY_BIND_FUNCTION(TestFunction1));
        REQUIRE(calledFunction == false);

        THEN("It should be callable")
        {

          auto result = interpreter.RunString("import testModule\ntestModule.TestFunction1()");
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

          auto result = interpreter.RunString("import testModule\ntestModule.myNamedFunction()");
          REQUIRE(calledFunction == true);
          REQUIRE(result.IsValid());
        }
      }
    }

    interpreter.Finalize();
  }
}
