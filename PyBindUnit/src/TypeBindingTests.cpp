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

class TestMemberType
{
public:

  TestMemberType()
  {
  }

  int GetVar() const
  {
    return m_Var;
  }
  void SetVar(int val)
  {
    m_Var = val;
  }
private:
  int m_Var = 100;
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

    WHEN("Binding a new type")
    {
      TypeObject<TestType> obj("TestType");

      mod.AddType(&obj);

      THEN("The type should be visible in python")
      {
        auto typeImportResult = interpreter.RunString("from testModule import TestType as myType");
        REQUIRE(typeImportResult.IsValid());
        REQUIRE(typeImportResult.IsNone());

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

SCENARIO("Type Member Binding", "[binding][type][member]")
{
  Interpreter interpreter;
  interpreter.Initialize();
  {
    Module mod("testModule");

    interpreter.RegisterModule(&mod);

    Object result;

    result = interpreter.RunString("import testModule");
    REQUIRE(result.IsValid());

    GIVEN("A bound type instance")
    {
      TypeObject<TestMemberType> obj("TestMemberType");
      obj.SetDefaultConstructor();
      obj.AddProperty(PY_BIND_PROPERTY(TestMemberType, Var));
      mod.AddType(&obj);

      result = interpreter.RunString("testobj = testModule.TestMemberType()");
      REQUIRE(result.IsValid());
      Object testobj = Dictionary::FromObject(interpreter.GetMainDict()).GetItem("testobj");
      auto* testobjPtr = testobj.ToValue<TestMemberType*>();

      REQUIRE(testobjPtr->GetVar() == 100);

      WHEN("Binding a property to the object")
      {
        THEN("The property should be settable")
        {
          result = interpreter.RunString("testobj.Var = 10");
          REQUIRE(result.IsValid());
          REQUIRE(result.IsNone());
          REQUIRE(testobjPtr->GetVar() == 10);
        }
      }

    }

  }
}
