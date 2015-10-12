// PyBindTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <PyBind.hpp>


int _tmain(int argc, wchar_t* argv[])
{
	pyb::Interpreter interpreter;

	interpreter.Initialize(argv[0]);

	interpreter.RunString( "print(\"Hello\")" );

	interpreter.Finalize();

    return 0;
}

