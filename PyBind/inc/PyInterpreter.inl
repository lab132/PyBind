#include "PyInterpreter.hpp"
#pragma once

namespace pyb
{

  inline
    Interpreter::Interpreter():
    m_Finalized(false)
  {

  }

  inline Interpreter::~Interpreter()
  {
    Finalize();
  }

  inline
    void Interpreter::Initialize(wchar_t* programName)
  {
    if (programName)
    {
      Py_SetProgramName(programName);
    }

    Py_Initialize();
  }

  inline
    void Interpreter::Finalize()
  {
    if(!m_Finalized)
    {
      m_GlobalsDict.Invalidate();
      m_Modules.clear();
      Py_Finalize();
      m_Finalized = true;
    }
  }

  inline
    void Interpreter::RunPyMain(int argc, wchar_t * argv [])
  {
    Py_Main(argc, argv);
  }

  inline
    Object pyb::Interpreter::RunString(const std::string & expression, const Object * globals, const Object * locals)
  {
    if(globals == nullptr)
    {
      globals = &GetMainDict();
    }
    if(locals == nullptr)
    {
      locals = &GetMainDict();
    }

    PyObject* obj = PyRun_String(
      expression.c_str(),
      Py_single_input,
      globals->ObjectPtr(),
      locals->ObjectPtr());
    if(obj == nullptr)
    {
      PyErr_Print();
    }
    return Object::FromNewRef(obj);
  }

  inline
    Object pyb::Interpreter::EvalString(const std::string & expression, const Object * globals, const Object * locals)
  {
    if(globals == nullptr)
    {
      globals = &GetMainDict();
    }
    if(locals == nullptr)
    {
      locals = &GetMainDict();
    }

    PyObject* obj = PyRun_String(
      expression.c_str(),
      Py_eval_input,
      globals->ObjectPtr(),
      locals->ObjectPtr());
    if(obj == nullptr)
    {
      PyErr_Print();
    }
    return Object::FromNewRef(obj);
  }

  inline
    Object Interpreter::RunFile(const std::string & fileName, const Object * globals, const Object * locals)
  {
    FILE* file = fopen(fileName.c_str(), "r");
    assert(file);

    if(globals == nullptr)
    {
      globals = &GetMainDict();
    }
    if(locals == nullptr)
    {
      locals = &GetMainDict();
    }

    return Object::FromNewRef(
      PyRun_FileEx(file, fileName.c_str(), Py_file_input, globals->ObjectPtr(), locals->ObjectPtr(), 1));
  }

  inline
    const Object& Interpreter::GetMainDict()
  {
    if(!m_GlobalsDict.IsValid())
    {
      auto* mainModule = PyImport_AddModule("__main__");

      assert(mainModule);

      auto* globalsDict = PyModule_GetDict(mainModule);
      m_GlobalsDict = Object::FromBorrowed(globalsDict);
    }

    return m_GlobalsDict;
  }

  inline
    void Interpreter::RegisterModule(Module * module)
  {
    if(std::find(m_Modules.begin(), m_Modules.end(), module) == m_Modules.end())
    {

      module->RegisterAtInterpreter(*this);

      m_Modules.push_back(module);
    }
  }

}
