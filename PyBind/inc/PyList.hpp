#pragma once

#include "PyObject.hpp"

namespace pyb
{
  class ListEntryRef;

  class List
  {
  public:

    List();
    List(const Object& obj);

    bool IsValid() const;

    template <typename T>
    T GetItem(size_t index) const;
    Object GetItemObject(size_t index) const;

    void SetItemObject(size_t index, Object& newValue);

    template <typename T>
    void SetItem(size_t index, const T& item);

    ListEntryRef operator[](size_t index) const;

    size_t Size() const;

    Object m_PyObject;

    static List FromObject(const Object& obj);
    static List Create();
  };

  /**
  @brief Wrapper class for array operator, allows assigning of the return value of the array index.

  This class is required since every value inside this array is stored as an PyObject.
  When returning an item from the dict with another type than Object we cannot change the returned value per reference,
  instead we need to exchange the stored PyObject with another one.
  This class wraps this behavior, so the dictionary array index operator returns this proxy class instead of the actual value.
  And when writing e.g. dict["myKey"]=10; it will translate it into something like: dict.SetItem<int>("myKey", 10);
  */
  class ListEntryRef
  {
  public:
    ListEntryRef(size_t key, const List& dict);

    template< typename T>
    void operator =(T obj);

    template<typename T>
    T GetValue() const;

    template<typename T>
    operator T() const;

    size_t m_Key;
    List m_List;
  };
}
