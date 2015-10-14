#pragma once

#include "PyBindCommon.hpp"
#include "PyObject.hpp"
#include <stdio.h>

namespace pyb
{
	class Interpreter
	{
	public:
		Interpreter();

		/**
		@brief Initializes the interpreter with the given programName.
		The interpreter is usable after this call.
		*/
		void Initialize( wchar_t* programName );

		void Finalize();

		void RunPyMain( int argc, wchar_t* argv [] );

		Object RunString( const std::string& expression, const Object* globals = nullptr, const Object* locals = nullptr );

		Object RunFile( const std::string& fileName, const Object* globals = nullptr, const Object* locals = nullptr );

		/**
		@brief Retrieves the module dictionary from the __main__ module in python.
		The returned pointer is owned by the Interpreter and is valid as long as the interpreter exists.
		
		@return A pointer to the owned module dictionary
		*/
		const Object* GetMainDict();

	private:
		Object m_GlobalsDict;
	};

	inline
	Interpreter::Interpreter()
	{

	}

	inline
	void Interpreter::Initialize( wchar_t* programName )
	{
		Py_SetProgramName( programName );

		Py_Initialize();
	}

	inline
	void Interpreter::Finalize()
	{
		Py_Finalize();
	}

	inline
	void Interpreter::RunPyMain( int argc, wchar_t * argv [] )
	{
		Py_Main( argc, argv );
	}
	
	inline
	Object pyb::Interpreter::RunString( const std::string & expression, const Object * globals, const Object * locals )
	{
		if( globals == nullptr )
		{
			globals = GetMainDict();
		}
		if( locals == nullptr )
		{
			locals = GetMainDict();
		}

		PyObject* obj = PyRun_String(
			expression.c_str(),
			Py_file_input,
			globals->ObjectPtr(),
			locals->ObjectPtr());
		if( obj == nullptr )
		{
			PyErr_Print();
		}
		return Object::FromNewRef(obj);
	}

	inline
	Object Interpreter::RunFile( const std::string & fileName, const Object * globals, const Object * locals )
	{
		FILE* file = fopen( fileName.c_str(), "r" );
		assert( file );

		if (globals == nullptr)
		{
			globals = GetMainDict();
		}
		if (locals == nullptr)
		{
			locals = GetMainDict();
		}

		return Object::FromNewRef( PyRun_FileEx( file, fileName.c_str(), Py_file_input, globals->ObjectPtr(), locals->ObjectPtr(), 1 ) );
	}

	inline
	const Object* Interpreter::GetMainDict()
	{
		if (!m_GlobalsDict.IsValid())
		{
			auto* mainModule = PyImport_AddModule( "__main__" );

			assert( mainModule );

			auto* globalsDict = PyModule_GetDict( mainModule );
			m_GlobalsDict = Object::Own( globalsDict );
		}

		return &m_GlobalsDict;
	}
}