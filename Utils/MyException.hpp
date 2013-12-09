#ifndef MYEXCEPTION_HPP
#define MYEXCEPTION_HPP

#include <exception>
#include <string>

class MyException : public std::exception
{
public:
  explicit MyException(const char* message);
  explicit MyException(const std::string& message);
  virtual ~MyException() throw (){}
  virtual const char* what() const throw();

protected:
  std::string m_msg;
};

#endif // MYEXCEPTION_HPP
