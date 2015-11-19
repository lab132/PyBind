#pragma once

#include "PyBindCommon.hpp"
#include "PyTypeBuilder.hpp"
#include "PyModule.hpp"
#include <vector>

namespace pyb
{
  class Module;
  class BaseTypeObject
  {
  public:

    virtual ~BaseTypeObject();

    std::vector<PyMethodDef> m_MethodDefs;
    std::vector<PyGetSetDef> m_GetSetDefs;

    void RegisterAtModule(Module& module);

    PyTypeObject m_Binding;
    Module* m_RegisteredModule = nullptr;
    void RegisterGetSetDefWithType(PyGetSetDef& newGetSetDef);
    void RegisterFunctionWithType(PyMethodDef& newGetSetDef);
  };

  template <typename T>
  class TypeObject : public BaseTypeObject
  {
  public:
    TypeObject(const char* name);

    void AddMethod(const BindDelegate& deleg);
    void AddProperty(const BindGetSetDelegate& deleg);

    void SetConstructor(const BindDelegate& deleg);
    void SetDefaultConstructor();

    friend class Module;
  private:

  };
}
