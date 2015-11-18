#include "catch.hpp"
#include "PyBindUnit.hpp"

class TestType
{
public:
  int GetVar() const
  {
    return m_Var;
  }
  void SetVar(int val)
  {
    m_Var = val;
  }
private:
  int m_Var;
};

SCENARIO("Type Binding", "[binding][type]")
{
  Interpreter interpreter;
  interpreter.Initialize();


  GIVEN("A bound module")
  {
    Module mod("testModule");

    interpreter.RegisterModule(&mod);

    auto importResult = interpreter.RunString("import testModule");

    REQUIRE(importResult.IsValid());

    WHEN("Binding a new module")
    {
      TypeObject<TestType> obj("TestType");

      mod.AddType(&obj);

      THEN("The type should be visible in python")
      {
        auto typeImportResult = interpreter.RunString("from testModule import TestType as myType");
        REQUIRE(typeImportResult.IsValid());

        AND_WHEN("Creating an instance of it")
        {
          auto creationResult = interpreter.EvalString("myType()");

          THEN("A type should be created")
          {
            REQUIRE(creationResult.IsValid());
            REQUIRE(!creationResult.IsNone());
            REQUIRE(creationResult.IsOfType(&obj));
          }
        }
      }
    }



  }
}
