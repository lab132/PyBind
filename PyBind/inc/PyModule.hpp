#pragma once

#include "PyBindCommon.hpp"
#include "PyObject.hpp"
#include <vector>

namespace pyb
{
  class BaseTypeObject;
  class Interpreter;
  struct BindDelegate;

  class Module
  {
  public:

    Module( const std::string& name, const std::string& documentation = "" );
    ~Module();

    const std::string& Name() const;
    const void SetName(const std::string& name);

    bool IsRegistered() const;
    void AddFunction( const BindDelegate& bindDelegate);
    void AddType(BaseTypeObject* typeObject);
    void RemoveType(BaseTypeObject* typeObject);
    const Object& GetObject() const;

    friend class Interpreter;
  private:

    void RegisterAtInterpreter( const Interpreter& interpreter );
    void AppendFunction( const PyMethodDef& def );

    void WriteMethodSentinel();

    std::string m_Name;
    std::string m_Documentation;

    Object m_Module;

    bool m_Registered;

    PyModuleDef m_ModuleDef;

    std::vector<PyMethodDef> m_MethodDefinitions;
    std::vector<BaseTypeObject*> m_RegisteredTypes;
  };


}
