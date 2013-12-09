#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cstring>

namespace Utility
{
  template<typename T>
  inline T getValueFromString(std::string const &s)
  {
    T           var(0);
    std::istringstream  iss;

    if (s != "")
      {
        iss.str(s);
        iss >> var;
      }
    return var;
  }

  template<>
  inline std::string    getValueFromString<std::string>(std::string const &s)
  {
    std::string tmp;

    tmp = s;
    return tmp;
  }

  template<typename T>
  inline T getValueFromString(const char *s)
  {
    std::string str;

    if (s)
      str = s;
    return getValueFromString<T>(str);
  }

  template<typename T>
  std::string   toString(T var)
  {
    std::ostringstream  oss;
    std::string     s;

    oss << var;
    s = oss.str();
    return s;
  }

  template<typename T>
  std::vector<T>  split(std::string &str, const char *key)
  {
    std::vector<T>    result;
    size_t      pos;
    std::string     sub;
    int         size;

    if (!key)
      return result;
    size = strlen(key);
    while ((pos = str.find(key)) != std::string::npos)
      {
        sub = str.substr(0, pos);
        str.erase(0, pos + size);
        if (!sub.empty())
          result.push_back(getValueFromString<T>(sub));
      }
    if (str != "")
      result.push_back(getValueFromString<T>(str));
    return result;
  }
  
  template<typename T>
  std::vector<T>  split(const char *str, const char *key)
  {
    std::string     st;

    if (!str)
      return split<T>(st, key);
    st = str;
    return split<T>(st, key);
  }

  template<typename T>
  std::vector<T>  split(const char *s, std::string const &key)
  {
    return split<T>(s, key.c_str());
  }

  template<typename T>
  std::vector<T>  split(std::string &str, std::string const &key)
  {
    return split<T>(str, key.c_str());
  }
  
  template<typename T>
  std::string   join(std::vector<T> const &c, std::string const &j)
  {
    return join<T>(c, j.c_str());
  }
  
  template<typename T>
  std::string   join(std::vector<T> const &c, const char *j)
  {
    std::ostringstream  os;
    unsigned int    i(0);
    std::string     str;

    if (!j)
      return str;
    while (i < c.size())
      {
        os.str("");
        os.clear();
        os << c[i++];
        str += os.str();
        if (i < c.size())
          str += j;
      }
    return str;
  }

  template<typename T>
  T getBetween(std::string const &c, const char *val1, const char *val2)
  {
    size_t      pos1, pos2;

    if (!val1 || !val2 ||
        (pos1 = c.find(val1)) == std::string::npos ||
        (pos2 = c.find(val2)) == std::string::npos)
      return getValueFromString<T>("");
    pos1 += strlen(val1);
    if (pos1 >= pos2)
      return getValueFromString<T>("");
    return getValueFromString<T>(c.substr(pos1, pos2 - pos1).c_str());
  }

  template<typename T>
  T getBetween(const char *st, const char *val1, const char *val2)
  {
    std::string str;

    if (st)
      str = st;
    return getBetween<T>(str, val1, val2);
  }

  template<typename T>
  std::vector<T>  getListOfItem(std::string st, const char *val1, const char *val2)
  {
    std::vector<T>    vec;
    std::string     str;
    size_t      find;
    int         length(val2 ? strlen(val2) : 0);

    str = getBetween<T>(st, val1, val2);
    for (;!str.empty(); str = getBetween<T>(st, val1, val2))
      {
        vec.push_back(str);
        if ((find = st.find(val1)) == std::string::npos)
          return vec;
        st.erase(find, str.size() + length);
      }
    return vec;
  }

  template<typename T>
  std::vector<T>  getListOfItem(const char *st, const char *val1, const char *val2)
  {
    std::string s;

    if (st)
      s = st;
    return getListOfItem<T>(s, val1, val2);
  }
  
  template<typename T>
  std::string replace(T &s, std::string const &toReplace,
             std::string const &replacement)
  {
    return replace<T>(s, toReplace.c_str(), replacement.c_str());
  }
  
  template<typename T>
  std::string replace(T &s, const char *toReplace,
             std::string const &replacement)
  {
    return replace<T>(s, toReplace, replacement.c_str());
  }
  
  template<typename T>
  std::string replace(T &s, std::string const &toReplace,
             const char *replacement)
  {
    return replace<T>(s, toReplace.c_str(), replacement);
  }
  
  template<typename T>
  std::string replace(T &s, const char *toReplace, const char *replacement)
  {
    std::vector<T>  vec;

    if (!toReplace || !replacement)
      return s;
    vec = split<T>(s, toReplace);
    return join<T>(vec, replacement);
  }
}
