#pragma once
namespace pyb
{
  template<typename T>
  struct PyTypeTrait
  {
  };

  template<>
  struct PyTypeTrait<int>
  {
    static constexpr const char* PyTypeString = "i";
    static constexpr const char* PyVerboseString = "int";
  };

  template<>
  struct PyTypeTrait<unsigned char>
  {
    static constexpr const char* PyTypeString = "b";
    static constexpr const char* PyVerboseString = "unsigned char";
  };

  template<>
  struct PyTypeTrait<char>
  {
    static constexpr const char* PyTypeString = "B";
    static constexpr const char* PyVerboseString = "char";
  };

  template<>
  struct PyTypeTrait<unsigned short>
  {
    static constexpr const char* PyTypeString = "H";
    static constexpr const char* PyVerboseString = "unsigned short";
  };

  template<>
  struct PyTypeTrait<short>
  {
    static constexpr const char* PyTypeString = "h";
    static constexpr const char* PyVerboseString = "short";
  };

  template<>
  struct PyTypeTrait<unsigned int>
  {
    static constexpr const char* PyTypeString = "I";
    static constexpr const char* PyVerboseString = "unsigned int";
  };

  template<>
  struct PyTypeTrait<unsigned long long>
  {
    static constexpr const char* PyTypeString = "K";
    static constexpr const char* PyVerboseString = "unsigned long long";
  };

  template<>
  struct PyTypeTrait<long long>
  {
    static constexpr const char* PyTypeString = "L";
    static constexpr const char* PyVerboseString = "long long";
  };

  template<>
  struct PyTypeTrait<bool>
  {
    static constexpr const char* PyTypeString = "p";
    static constexpr const char* PyVerboseString = "bool";
  };

  template<>
  struct PyTypeTrait<const char*>
  {
    static constexpr const char* PyTypeString = "s";
    static constexpr const char* PyVerboseString = "string";
  };

  template<>
  struct PyTypeTrait<void>
  {
    static constexpr const char* PyTypeString = "";
    static constexpr const char* PyVerboseString = "void";
  };

  template<>
  struct PyTypeTrait<float>
  {
    static constexpr const char* PyTypeString = "f";
    static constexpr const char* PyVerboseString = "float";
  };

  template<>
  struct PyTypeTrait<double>
  {
    static constexpr const char* PyTypeString = "d";
    static constexpr const char* PyVerboseString = "double";
  };

  template<>
  struct PyTypeTrait<Object>
  {
    static constexpr const char* PyTypeString = "O";
    static constexpr const char* PyVerboseString = "object";
  };

  template<>
  struct PyTypeTrait<std::string>
  {
    static constexpr const char* PyTypeString = "s";
    static constexpr const char* PyVerboseString = "string";
  };

  template<typename T>
  struct PyTypeTrait<T*>
  {
    static constexpr const char* PyTypeString = "O";
    static constexpr const char* PyVerboseString = "pointer";
  };
}
