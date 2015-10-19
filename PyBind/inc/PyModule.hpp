#pragma once

#include "PyBindCommon.hpp"
#include "PyTypeBuilder.hpp"
#include <vector>

namespace pyb
{
  class Interpreter;

  class Module
  {
  public:

    Module( const std::string& name, const std::string& documentation = "" );

    const std::string& Name() const;
    const void SetName(const std::string& name);

    bool IsRegistered() const;
    void AddFunction( const BindDelegate& bindDelegate);

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
  };

  inline pyb::Module::Module( const std::string& name, const std::string& documentation ) :
    m_Name(name),
    m_Documentation(documentation),
    m_Registered(false)
  {
    m_MethodDefinitions.push_back( {nullptr,nullptr,0,nullptr} );
  }

  inline const void pyb::Module::SetName( const std::string & name )
  {
    assert( m_Registered == false );
    m_Name = name;
  }

  inline bool Module::IsRegistered() const
  {
    return m_Registered;
  }

  inline const std::string & Module::Name() const
  {
    return m_Name;
  }

  inline
  void pyb::Module::RegisterAtInterpreter( const Interpreter & interpreter )
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

    m_Module = Object::FromNewRef( PyModule_Create( &m_ModuleDef ) );

    Object moduleDict = Object::FromBorrowed( PyImport_GetModuleDict() );

    PyDict_SetItemString( moduleDict.ObjectPtr(), m_Name.c_str(), m_Module.ObjectPtr() );

  }

  inline void Module::AppendFunction( const PyMethodDef & def )
  {
    memcpy( m_MethodDefinitions.data() + m_MethodDefinitions.size() - 1, &def, sizeof( PyMethodDef ) );
    WriteMethodSentinel();
  }

  inline void Module::WriteMethodSentinel()
  {
    m_MethodDefinitions.push_back( { nullptr,nullptr,0,nullptr } );
  }

  inline
  void pyb::Module::AddFunction( const BindDelegate& bindDelegate )
  {

    AppendFunction( { bindDelegate.Name, bindDelegate.Function, METH_VARARGS| METH_KEYWORDS, bindDelegate.Name } );
    // Redefine all function, because the vector might have resized and definitions have moved in memory
    int result = PyModule_AddFunctions( m_Module.ObjectPtr(), m_MethodDefinitions.data() );

    assert( result == 0 );


  }

}
