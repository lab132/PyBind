#pragma once

#include "PyBindCommon.hpp"
#include "PyObject.hpp"

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
		Py_INCREF( Py_None );

		auto* mainModule = PyImport_AddModule( "__main__" );

		assert( mainModule );

		auto* globalsDict = PyModule_GetDict(mainModule);

		assert( globalsDict );
		PyObject* obj = PyRun_String(
			expression.c_str(),
			Py_file_input,
			( globals != nullptr ? globals->ObjectPtr() : globalsDict),
			(locals != nullptr ? locals->ObjectPtr() : globalsDict ) );
		Py_DECREF( Py_None );
		if( obj == nullptr )
		{
			PyErr_Print();
		}
		return Object(obj);
	}
}