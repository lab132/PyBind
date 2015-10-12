#pragma once

#include "PyBindCommon.hpp"

namespace pyb
{
	class Object
	{
	public:
		Object( PyObject* obj );
		Object( const Object& obj );
		Object( Object&& other );
		Object();
		~Object();

		Object& operator =( const Object& other );

		PyObject* ObjectPtr() const;

		PyObject* m_PyObject;

		static Object own( PyObject* pyObject );
	};

	inline
	Object::Object( PyObject * obj ) :
		m_PyObject(obj)
	{
		assert( m_PyObject );
	}

	inline
	Object::Object( const Object & obj ) :
		m_PyObject(obj.m_PyObject)
	{
		Py_XINCREF( m_PyObject );
	}

	inline
	Object::Object( Object && other ) :
		m_PyObject(other.m_PyObject)
	{
		other.m_PyObject = nullptr;
	}

	inline Object::Object() :
		m_PyObject(nullptr)
	{
	}

	inline
	Object::~Object()
	{
		Py_XDECREF( m_PyObject );
		m_PyObject = nullptr;
	}

	inline
	PyObject* pyb::Object::ObjectPtr() const
	{
		return m_PyObject;
	}

	inline
	Object Object::own( PyObject * pyObject )
	{
		assert( pyObject );
		Object obj = Object( pyObject );
		Py_INCREF( pyObject );
		return obj;
	}

	inline
	Object & pyb::Object::operator=( const Object & other )
	{
		m_PyObject = other.m_PyObject;
		Py_XINCREF( m_PyObject );

		return *this;
	}
}