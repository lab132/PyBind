#pragma once

#include "PyBindCommon.hpp"

namespace pyb
{
	class Object
	{
	public:
		Object( const Object& obj );
		Object( Object&& other );
		Object();
		~Object();

		Object& operator =( const Object& other );

		PyObject* ObjectPtr() const;

		PyObject* m_PyObject;

		bool IsValid() const;

		static Object Own( PyObject* pyObject );
		static Object FromNewRef( PyObject* pyObject );
	};

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

	inline bool Object::IsValid() const
	{
		return m_PyObject != nullptr;
	}

	inline
	Object Object::Own( PyObject * pyObject )
	{
		Object obj;
		obj.m_PyObject = pyObject;
		Py_XINCREF( pyObject );
		return obj;
	}

	inline
	Object Object::FromNewRef( PyObject* pyObject )
	{
		Object obj;
		obj.m_PyObject = pyObject;
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