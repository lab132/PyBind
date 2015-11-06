#include "catch.hpp"

#include "PyBindUnit.hpp"

bool calledFunction = false;

void TestFunction1();

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
