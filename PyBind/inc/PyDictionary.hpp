#pragma once

#include "PyObject.hpp"

namespace pyb
{
  class DictionaryEntryRef;
  class Dictionary
  {
  public:



    Dictionary(const Object& obj);

    static Dictionary FromObject(const Object& obj);
    static Dictionary Create();

    bool ContainsKey(const Object& obj) const;
    bool ContainsKey(const std::string& string) const;

    bool DeleteItem(const Object& key) const;
    bool DeleteItem(const std::string& key) const;

    Object GetItem(const Object& key) const;
    Object GetItem(const std::string& key) const;

    bool SetObject(const Object& key, const Object& value) const;
    bool SetObject(const std::string& key, const Object& value) const;

    DictionaryEntryRef operator[](const std::string& key);

    template<typename T>
    bool SetItem(const std::string& key, T value);
    template<typename T>
    bool SetItem(const Object& key, T value);

    template<typename T>
    T GetItem(const std::string& key) const;
    template<typename T>
    T GetItem(const Object& key) const;

    void Clear();

    size_t Size() const;

    Object m_Dictionary;
  };

  /**
  @brief Wrapper class for array operator, allows assigning of the return value of the array index.

  This class is required since every value inside this array is stored as an PyObject.
  When returning an item from the dict with another type than Object we cannot change the returned value per reference,
  instead we need to exchange the stored PyObject with another one.
  This class wraps this behavior, so the dictionary array index operator returns this proxy class instead of the actual value.
  And when writing e.g. dict["myKey"]=10; it will translate it into something like: dict.SetItem<int>("myKey", 10);
  */
  class DictionaryEntryRef
  {
  public:
    DictionaryEntryRef(const Object& key, const Dictionary& dict);

    template< typename T>
    void operator =(const T& obj);

    template<typename T>
    T GetValue() const;

    template<typename T>
    operator T() const;

    Object m_Key;
    Dictionary m_Dict;
  };
}
