#pragma once

#include "PyObject.hpp"

namespace pyb
{
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

    template<typename T>
    bool SetItem(const std::string& key, T value);
    template<typename T>
    bool SetItem(const Object& key, T value);

    template<typename T>
    T GetItem(const std::string& key);
    template<typename T>
    T GetItem(const Object& key);

    void Clear();

    size_t Size() const;

    Object m_Dictionary;
  };
}
