#pragma once

#include "PyBindCommon.hpp"
#include "PyTypeBuilder.hpp"
#include "PyModule.hpp"

namespace pyb
{

  template<typename T>
  class TypeObject
  {
  public:
    TypeObject(const char* name);



    friend class Module;
  private:


    void RegisterAtModule(Module& module);

    PyTypeObject m_Binding;
  };

  template<typename T>
  inline
  pyb::TypeObject<T>::TypeObject(const char * name)
  {

  }

  template<typename T>
  inline
  void TypeObject<T>::RegisterAtModule(Module & module)
  {
    m_Binding = {
      PyVarObject_HEAD_INIT(NULL, 0),
      name,
      sizeof(BaseBindObject<T>),
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

    m_Binding.tp_new = [](PyTypeObject* classDef, PyObject* args, PyObject* kwArgs)
    {
      PYB_ASSERT(classDef);

      BaseBindObject<T>* newObj = classDef->tp_alloc(classDef, 0);

      newObj->ptr = T();

      return reinterpret_cast<PyObject*>(newObj);
    };

    PyType_Ready(&m_Binding);
  }
}
