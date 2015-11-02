#pragma once

#include "PyObject.hpp"

namespace pyb
{
  class Dictionary
  {
  public:

    Dictionary(const Object& obj);

    static Dictionary FromObject(const Object& obj);

    bool ContainsKey(const Object& obj) const;
    bool ContainsKey(const std::string& string) const;

    Object GetItem(const Object& key) const;
    Object GetItem(const std::string& key) const;

    bool SetObject(const Object& key, const Object& value) const;
    bool SetObject(const std::string& key, const Object& value) const;

    template<typename T>
    bool SetItem(const std::string& key, T value);

    void Clear();

  private:
    Object m_Dictionary;
  };
}
