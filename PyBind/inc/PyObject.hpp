#pragma once

#include "PyBindCommon.hpp"

namespace pyb
{
	class Object
	{
	public:
		Object( PyObject* obj );
		~Object();

		PyObject* ObjectPtr() const;

		PyObject* m_PyObject;
	};

	inline
	pyb::Object::Object( PyObject * obj ) :
		m_PyObject(obj)
	{
		assert( m_PyObject );
		Py_INCREF( m_PyObject );
	}

	inline
	pyb::Object::~Object()
	{
		Py_DECREF( m_PyObject );
		m_PyObject = nullptr;
	}

	inline
	PyObject* pyb::Object::ObjectPtr() const
	{
		return m_PyObject;
	}
}