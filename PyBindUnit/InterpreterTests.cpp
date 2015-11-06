
#include "catch.hpp"

#include "PyBindUnit.hpp"

SCENARIO("Simple interpreter test", "[interpreter]")
{
  GIVEN("An initialized interpreter")
  {
    Interpreter interpreter;

    interpreter.Initialize();

    WHEN("Running a simple command")
    {
      auto result = interpreter.RunString("print('Hello')");

      THEN("The result should be valid")
      {
        REQUIRE(result.IsValid());
      }
    }

    WHEN("Running an invalid command")
    {
      auto result = interpreter.RunString("asldjhgalskdgj");

      THEN("The result should be valid")
      {
        REQUIRE(result.IsValid());
      }
    }

    interpreter.Finalize();
  }
}
