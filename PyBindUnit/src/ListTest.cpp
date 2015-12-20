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

SCENARIO("List access", "[binding][list][access]")
{
  Interpreter interpreter;
  interpreter.Initialize();
  GIVEN("A list created in python")
  {
    Object obj = interpreter.EvalString("[1,2,3,'hello']");
    REQUIRE(obj.IsValid());
    REQUIRE(obj.IsList());
    List list = obj.ToList();
    WHEN("Accessing the list")
    {
      REQUIRE(list.Size() == 4);
      int val1 = list.GetItem<int>(0);
      int val2 = list.GetItem<int>(1);
      int val3 = list.GetItem<int>(2);
      std::string val4 = list.GetItem<std::string>(3);

      THEN("The values should have the expected value")
      {
        REQUIRE(val1 == 1);
        REQUIRE(val2 == 2);
        REQUIRE(val3 == 3);
        REQUIRE(val4 == "hello");

        AND_WHEN("Modifying the values in the list")
        {
          list.SetItem<std::string>(0, "bye");
          THEN("The array should be changed")
          {
            REQUIRE("bye" == list.GetItem<std::string>(0));
          }
        }
      }
    }
  }
}

SCENARIO("List index operator", "[binding][list][access]")
{
  Interpreter interpreter;
  interpreter.Initialize();
  GIVEN("A list created in python")
  {
    Object obj = interpreter.EvalString("[1,2,3,'hello']");
    REQUIRE(obj.IsValid());
    REQUIRE(obj.IsList());
    List list = obj.ToList();
    WHEN("Accessing the list")
    {
      REQUIRE(list.Size() == 4);
      int val1 = list[0];
      int val2 = list[1];
      int val3 = list[2];
      std::string val4 = list[3];

      THEN("The values should have the expected value")
      {
        REQUIRE(val1 == 1);
        REQUIRE(val2 == 2);
        REQUIRE(val3 == 3);
        REQUIRE(val4 == "hello");

        AND_WHEN("Modifying the values in the list")
        {
          list.SetItem<std::string>(0, "bye");
          THEN("The array should be changed")
          {
            REQUIRE("bye" == list.GetItem<std::string>(0));
          }
        }
      }
    }
  }
}
