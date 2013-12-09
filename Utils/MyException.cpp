#include "MyException.hpp"

MyException::MyException(const char *msg) : m_msg(msg ? msg : "")
{
}

MyException::MyException(const std::string &message) : m_msg(message)
{
}

const char  *MyException::what() const throw()
{
  return m_msg.c_str();
}
