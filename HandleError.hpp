#ifndef HANDLEERROR_HPP
#define HANDLEERROR_HPP

#include <string>

class HandleError
{
public:
  static void showError(std::string const&);
  static void showError(const char*);
};

#endif // HANDLEERROR_HPP
