#include "catch.hpp"

#include "PyBindUnit.hpp"

SCENARIO("List creation", "[binding][list][create]")
{
  GIVEN("An interpreter")
  {
    Interpreter interpreter;
    interpreter.Initialize();

    WHEN("Creating a new list")
    {
      List list = List::Create();
      THEN("It should be valid")
      {
        REQUIRE(list.IsValid());
        REQUIRE(list.Size() == 0);
      }
    }
  }
}
