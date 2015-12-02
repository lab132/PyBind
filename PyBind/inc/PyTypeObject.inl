#include "PyTypeObject.hpp"
#pragma once

namespace pyb
{
  template<typename T>
  inline
    pyb::TypeObject<T>::TypeObject(const char * name)
  {
    m_Binding = {
      PyVarObject_HEAD_INIT(NULL, 0)
      name,
      sizeof(BaseBindObject),
      0,                         /* tp_itemsize */
      0,                         /* tp_dealloc */
      0,                         /* tp_print */
      0,                         /* tp_getattr */
      0,                         /* tp_setattr */
      0,                         /* tp_reserved */
      0,                         /* tp_repr */
      0,                         /* tp_as_number */
      0,                         /* tp_as_sequence */
      0,                         /* tp_as_mapping */
      0,                         /* tp_hash  */
      0,                         /* tp_call */
      0,                         /* tp_str */
      0,                         /* tp_getattro */
      0,                         /* tp_setattro */
      0,                         /* tp_as_buffer */
      Py_TPFLAGS_DEFAULT,        /* tp_flags */
      "",                        /* tp_doc */
    };

    m_MethodDefs.push_back({nullptr, nullptr, 0, nullptr});
    m_GetSetDefs.push_back({nullptr, nullptr, 0, nullptr, nullptr});

    m_Binding.tp_new = [](PyTypeObject* classDef, PyObject* args, PyObject* kwArgs)
    {
      PYB_ASSERT(classDef);

      BaseBindObject* newObj = reinterpret_cast<BaseBindObject*>(classDef->tp_alloc(classDef, 0));

      newObj->ptr = malloc(sizeof(T));

      return reinterpret_cast<PyObject*>(newObj);
    };

    m_Binding.tp_dealloc = [](PyObject* obj)
    {
      PYB_ASSERT(obj);

      BaseBindObject* typedObj = reinterpret_cast<BaseBindObject*>(obj);
      free(typedObj->ptr);
      typedObj->ptr = nullptr;

      Py_TYPE(obj)->tp_free(obj);
    };
  }

  template<typename T>
  inline
    void TypeObject<T>::AddMethod(const BindDelegate & deleg)
  {
    m_MethodDefs[m_MethodDefs.size() - 1] = {deleg.Name, deleg.Function, METH_VARARGS | METH_KEYWORDS, deleg.Name};
    m_MethodDefs.push_back({nullptr, nullptr, 0, nullptr});

    // Are we already registered and did our vector move due to resizing?
    // Then we need to redefine all already defined properties
    if(m_Binding.tp_methods != m_MethodDefs.data() && m_RegisteredModule)
    {
      auto typeDict = Object::FromBorrowed(m_Binding.tp_dict).ToDictionary();

      //Excluding new delegate and sentinel
      for(size_t i = 0; i < m_MethodDefs.size() - 2; i++)
      {
        PyMethodDef& newMethodDef = m_MethodDefs[i];
        RegisterFunctionWithType(newMethodDef);
      }
    }

    if(m_RegisteredModule)
    {
      RegisterFunctionWithType(m_MethodDefs[m_MethodDefs.size() - 2]);
    }

    m_Binding.tp_methods = m_MethodDefs.data();
  }

  template<typename T>
  inline void TypeObject<T>::AddProperty(const BindGetSetDelegate & deleg)
  {
    m_GetSetDefs[m_MethodDefs.size() - 1] = PyGetSetDef{
      const_cast<char*>(deleg.Name),
      deleg.Getter,
      deleg.Setter,
      const_cast<char*>(deleg.Name),
      nullptr
    };
    m_GetSetDefs.push_back({nullptr, nullptr, 0, nullptr, nullptr});

    // Are we already registered and did our vector move due to resizing?
    // Then we need to redefine all already defined properties
    if (m_Binding.tp_getset != m_GetSetDefs.data() && m_RegisteredModule)
    {
      //Excluding new delegate and sentinel
      for(size_t i = 0; i < m_GetSetDefs.size() - 2; i++)
      {
        auto& newGetSetDef = m_GetSetDefs[i];
        RegisterGetSetDefWithType(newGetSetDef);

      }
    }

    if (m_RegisteredModule)
    {
      RegisterGetSetDefWithType(m_GetSetDefs[m_GetSetDefs.size() - 2]);
    }

    m_Binding.tp_getset = m_GetSetDefs.data();
  }

  template<typename T>
  inline
    void TypeObject<T>::SetConstructor(const BindDelegate & deleg)
  {
    m_Binding.tp_init = reinterpret_cast<initproc>(deleg.Function);
  }

  template<typename T>
  inline void TypeObject<T>::SetDefaultConstructor()
  {
    initproc func = [](PyObject* self, PyObject* args, PyObject* keywords)
    {
      BaseBindObject* newObj = reinterpret_cast<BaseBindObject*>(self);

      T* obj = reinterpret_cast<T*>(newObj->ptr);

      new (obj)T();

      return 0;
    };

    m_Binding.tp_init = func;
  }

  inline BaseTypeObject::~BaseTypeObject()
  {
    if (m_RegisteredModule)
    {
      // AFAIK this will remove any reference to this type from the python interpreter,
      // which is probably what we want, since we want to remove the type completely
      // out of python (since after this dtor the reference to this object will be invalid memory)
      // However there is no documentation at all on how to remove a type object from python
      // (probably there is no way, since the tutorials expect you too create this as a global variable)

      // Remove from module dictionary
      m_RegisteredModule->RemoveType(this);

      // Remove from internal type dictionary
      Object::FromBorrowed(m_Binding.tp_dict).ToDictionary().Clear();

      // Remove from heap tracking
      _Py_ForgetReference(reinterpret_cast<PyObject*>(&m_Binding));
      m_RegisteredModule = nullptr;
    }
  }

  inline
    void BaseTypeObject::RegisterAtModule(Module & module)
  {
    PyType_Ready(&m_Binding);
    Py_INCREF(&m_Binding);
    m_RegisteredModule = &module;
  }

  inline
  void BaseTypeObject::RegisterGetSetDefWithType(PyGetSetDef& newGetSetDef)
  {
    Dictionary tpDict = Object::FromBorrowed(m_Binding.tp_dict).ToDictionary();
    auto newGetSet = Object::FromNewRef(PyDescr_NewGetSet(&m_Binding, &newGetSetDef));
    tpDict.SetItem(newGetSetDef.name, newGetSet);
  }
  inline void BaseTypeObject::RegisterFunctionWithType(PyMethodDef & methodDef)
  {

    Object descr;
    Dictionary typeDict = Object::FromBorrowed(m_Binding.tp_dict).ToDictionary();
    if(typeDict.ContainsKey(methodDef.ml_name) &&
      !(methodDef.ml_flags & METH_COEXIST))
    {
      return;
    }
    if(methodDef.ml_flags & METH_CLASS)
    {
      if(methodDef.ml_flags & METH_STATIC)
      {
        PyErr_SetString(PyExc_ValueError,
          "method cannot be both class and static");
        return;
      }
      descr = Object::FromNewRef(PyDescr_NewClassMethod(&m_Binding, &methodDef));
    }
    else if(methodDef.ml_flags & METH_STATIC)
    {
      Object cfunc = Object::FromNewRef(
        PyCFunction_NewEx(&methodDef, reinterpret_cast<PyObject*>(&m_Binding), nullptr));
      if(!cfunc.IsValid())
        return;
      descr = Object::FromNewRef(PyStaticMethod_New(cfunc.ObjectPtr()));
    }
    else
    {
      descr = Object::FromNewRef(PyDescr_NewMethod(&m_Binding, &methodDef));
    }
    if(!descr.IsValid())
    {
      return;
    }
    typeDict.SetItem(methodDef.ml_name, descr);
  }
}
