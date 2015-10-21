#pragma once

#include "PyBindCommon.hpp"
#include "PyTypeBuilder.hpp"
#include "PyModule.hpp"
#include <vector>

namespace pyb
{
  class Module;
  class BaseTypeObject
  {
  public:

    std::vector<PyMethodDef> m_MethodDefs;

    void RegisterAtModule(Module& module);

    PyTypeObject m_Binding;
  };

  template <typename T>
  class TypeObject : public BaseTypeObject
  {
  public:
    TypeObject(const char* name);

    void AddMethod(const BindDelegate& deleg);

    friend class Module;
  private:

  };

  template<typename T>
  inline
  pyb::TypeObject<T>::TypeObject(const char * name)
  {
    m_Binding = {
      PyVarObject_HEAD_INIT(NULL, 0)
      name,
      sizeof(T),
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

    m_Binding.tp_new = [](PyTypeObject* classDef, PyObject* args, PyObject* kwArgs)
    {
      PYB_ASSERT(classDef);

      BaseBindObject* newObj = reinterpret_cast<BaseBindObject*>( classDef->tp_alloc(classDef, 0));

      newObj->ptr = new T();

      return reinterpret_cast<PyObject*>(newObj);
    };
  }

  template<typename T>
  inline
  void TypeObject<T>::AddMethod(const BindDelegate & deleg)
  {
    m_MethodDefs[m_MethodDefs.size() - 1] = {deleg.Name, deleg.Function, METH_VARARGS | METH_KEYWORDS, deleg.Name};
    m_MethodDefs.push_back({nullptr, nullptr, 0, nullptr});

    m_Binding.tp_methods = m_MethodDefs.data();
  }

  inline
  void BaseTypeObject::RegisterAtModule(Module & module)
  {
    PyType_Ready(&m_Binding);
  }
}
