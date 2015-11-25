#include "catch.hpp"

#include "PyBindUnit.hpp"

SCENARIO("Dictionary Creation", "[binding][dictionary][create]")
{
  GIVEN("An interpreter")
  {
    Interpreter interpreter;
    interpreter.Initialize();

    WHEN("Creating a new dictionary")
    {
      Dictionary dict = Dictionary::Create();
      REQUIRE(dict.m_Dictionary.IsValid());
      REQUIRE(!dict.m_Dictionary.IsNone());
      REQUIRE(dict.m_Dictionary.IsDictionary());
      REQUIRE(dict.Size() == 0);
    }
  }
}

SCENARIO("Dictionary Insertion", "[binding][dictionary][insert]")
{
  GIVEN("An empty dictionary")
  {
    Interpreter interpreter;
    interpreter.Initialize();
    Dictionary dict = Dictionary::Create();
    REQUIRE(dict.m_Dictionary.IsValid());
    REQUIRE(!dict.m_Dictionary.IsNone());
    REQUIRE(dict.m_Dictionary.IsDictionary());
    REQUIRE(dict.Size() == 0);

    WHEN("Inserting a string key/value")
    {
      REQUIRE(!dict.ContainsKey("testKey"));
      REQUIRE(dict.SetItem("testKey", 5));
      REQUIRE(dict.Size() == 1);
      THEN("It should be saved in the dictionary")
      {
        REQUIRE(dict.ContainsKey("testKey"));
        REQUIRE(dict.GetItem<int>("testKey") == 5);

        AND_WHEN("Deleting the item")
        {
          REQUIRE(dict.DeleteItem("testKey"));

          THEN("It should be removed")
          {
            REQUIRE(dict.Size() == 0);

            REQUIRE(!dict.ContainsKey("testKey"));
          }
        }
      }

    }

    WHEN("Inserting a object key/value")
    {
      REQUIRE(!dict.ContainsKey("testKey"));
      Object key = BuildValue(1);
      REQUIRE(dict.SetItem(key, 5));
      THEN("It should be saved in the dictionary")
      {
        REQUIRE(dict.Size() == 1);
        REQUIRE(dict.ContainsKey(key));
        REQUIRE(dict.ContainsKey(BuildValue(1)));
        REQUIRE(dict.GetItem<int>(key) == 5);

        AND_WHEN("Deleting the item")
        {
          REQUIRE(dict.DeleteItem(key));

          THEN("It should be removed")
          {
            REQUIRE(!dict.ContainsKey(key));
            REQUIRE(dict.Size() == 0);
          }
        }
      }

    }
  }
}

SCENARIO("Dictionary deletion", "[binding][dictionary][delete]")
{
  GIVEN("An dictionary filled with values")
  {
    Interpreter interpreter;
    interpreter.Initialize();
    Dictionary dict = Dictionary::Create();
    REQUIRE(dict.m_Dictionary.IsValid());
    REQUIRE(!dict.m_Dictionary.IsNone());
    REQUIRE(dict.m_Dictionary.IsDictionary());
    REQUIRE(dict.SetItem("testKey", 5));
    REQUIRE(dict.SetItem("testKey2", 10));
    Object key = BuildValue(1);
    REQUIRE(dict.SetItem(key, 20));
    REQUIRE(dict.GetItem<int>("testKey") == 5);
    REQUIRE(dict.GetItem<int>("testKey2") == 10);
    REQUIRE(dict.GetItem<int>(key) == 20);
    REQUIRE(dict.Size() == 3);

    WHEN("Deleting a value")
    {
      REQUIRE(dict.ContainsKey("testKey"));
      REQUIRE(dict.DeleteItem("testKey"));
      THEN("It should be removed from the dictionary")
      {
        REQUIRE(dict.Size() == 2);
        REQUIRE(!dict.ContainsKey("testKey"));
        REQUIRE(dict.ContainsKey("testKey2"));

        AND_WHEN("Deleting the item again")
        {
          REQUIRE(!dict.DeleteItem("testKey"));

          THEN("It should be still removed")
          {
            REQUIRE(dict.Size() == 2);
            REQUIRE(!dict.ContainsKey("testKey"));
          }
        }
      }

    }

    WHEN("Deleting a key object value")
    {
      REQUIRE(dict.ContainsKey(key));
      REQUIRE(dict.DeleteItem(key));
      THEN("It should be removed from the dictionary")
      {
        REQUIRE(dict.Size() == 2);
        REQUIRE(!dict.ContainsKey(key));
        REQUIRE(dict.ContainsKey("testKey2"));

        AND_WHEN("Deleting the item again")
        {
          REQUIRE(!dict.DeleteItem(key));

          THEN("It should be still removed")
          {
            REQUIRE(dict.Size() == 2);
            REQUIRE(!dict.ContainsKey(key));
          }
        }
      }

    }

    WHEN("Clearing the dictionary")
    {
      dict.Clear();
      THEN("It should be empty")
      {
        REQUIRE(dict.Size() == 0);
        REQUIRE(!dict.ContainsKey("testKey"));
        REQUIRE(!dict.ContainsKey("testKey2"));
        REQUIRE(!dict.ContainsKey(key));
      }
    }
  }
}

SCENARIO("Dictionary wrapping", "[binding][dictionary]")
{
  GIVEN("An interpreter")
  {
    Interpreter interpreter;
    interpreter.Initialize();
    WHEN("Creating a dict from python")
    {
      Object result = interpreter.EvalString("{'test':'hello',1:2}");
      REQUIRE(result.IsValid());
      REQUIRE(!result.IsNone());
      REQUIRE(result.IsDictionary());
      Dictionary dict = result.ToDictionary();

      THEN("It should have the keys set in python")
      {
        REQUIRE(dict.ContainsKey("test"));
        REQUIRE(dict.ContainsKey(BuildValue(1)));
        REQUIRE(dict.GetItem<std::string>("test") == "hello");
        REQUIRE(dict.GetItem<int>(BuildValue(1)) == 2);
      }

    }
  }
}
