#include "PyModule.hpp"
#pragma once

namespace pyb
{

  inline
    pyb::Module::Module(const std::string& name, const std::string& documentation) :
    m_Name(name),
    m_Documentation(documentation),
    m_Registered(false),
    m_RegisteredTypes()
  {
    m_MethodDefinitions.push_back({nullptr,nullptr,0,nullptr});
  }

  inline Module::~Module()
  {
    if (m_Registered)
    {

      Object moduleDict = Object::FromBorrowed(PyImport_GetModuleDict());

      PyDict_SetItemString(moduleDict.ObjectPtr(), m_Name.c_str(), Py_None);
      m_Registered = false;
    }
  }

  inline
    const void pyb::Module::SetName(const std::string & name)
  {
    assert(m_Registered == false);
    m_Name = name;
  }

  inline
    bool Module::IsRegistered() const
  {
    return m_Registered;
  }

  inline
    const std::string & Module::Name() const
  {
    return m_Name;
  }

  inline
    void pyb::Module::RegisterAtInterpreter(const Interpreter & interpreter)
  {
    m_Registered = true;

    m_ModuleDef = {
      PyModuleDef_HEAD_INIT,
      m_Name.c_str(),
      m_Documentation.c_str(),
      0,
      nullptr, // Method def
      nullptr, // Slots
      nullptr, // traverse
      nullptr, // inquiry
      nullptr // freefunc
    };

    m_Module = Object::FromNewRef(PyModule_Create(&m_ModuleDef));

    Object moduleDict = Object::FromBorrowed(PyImport_GetModuleDict());

    PyDict_SetItemString(moduleDict.ObjectPtr(), m_Name.c_str(), m_Module.ObjectPtr());

  }

  inline
    void Module::AppendFunction(const PyMethodDef & def)
  {
    memcpy(m_MethodDefinitions.data() + m_MethodDefinitions.size() - 1, &def, sizeof(PyMethodDef));
    WriteMethodSentinel();
  }

  inline
    void Module::WriteMethodSentinel()
  {
    m_MethodDefinitions.push_back({nullptr,nullptr,0,nullptr});
  }

  inline
    void pyb::Module::AddFunction(const BindDelegate& bindDelegate)
  {

    AppendFunction({bindDelegate.Name, bindDelegate.Function, METH_VARARGS | METH_KEYWORDS, bindDelegate.Name});
    // Redefine all function, because the vector might have resized and definitions have moved in memory
    int result = PyModule_AddFunctions(m_Module.ObjectPtr(), m_MethodDefinitions.data());

    assert(result == 0);


  }

  inline
    const Object& pyb::Module::GetObject() const
  {
    return m_Module;
  }

  inline
    void pyb::Module::AddType(BaseTypeObject* typeObject)
  {
    typeObject->RegisterAtModule(*this);

    m_RegisteredTypes.push_back(typeObject);
    PyModule_AddObject(
      m_Module.ObjectPtr(),
      typeObject->m_Binding.tp_name,
      reinterpret_cast<PyObject*> (&typeObject->m_Binding));

  }
  inline void Module::RemoveType(BaseTypeObject * typeObject)
  {
    auto result = std::find(m_RegisteredTypes.begin(), m_RegisteredTypes.end(), typeObject);
    PYB_ASSERT(result != m_RegisteredTypes.end());

    m_RegisteredTypes.erase(result);

    Dictionary::FromObject(
      Object::FromBorrowed(
        PyModule_GetDict(
          m_Module.ObjectPtr()))).
      SetItem(
        typeObject->m_Binding.tp_name,
        Object::FromBorrowed(Py_None));
  }
}
