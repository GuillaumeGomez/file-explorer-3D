#include "HandleError.hpp"
#include <iostream>
#if defined(WIN32)
#include <windows.h>
#endif
#include "MyWindow.hpp"

using namespace std;

void HandleError::showError(const char *s)
{
  if (!s)
    return;
  if (!MyWindow::displayErrorMessage("Error", s))
    {
#if defined(WIN32)
      const WCHAR *pwcsName;
      int nChars = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
      pwcsName = new WCHAR[nChars];
      MultiByteToWideChar(CP_ACP, 0, s, -1, (LPWSTR)pwcsName, nChars);
      MessageBox(NULL, pwcsName, L"Error !", MB_OK);
      delete[] pwcsName;
#else
      cerr << "Error: " << s << endl;
#endif
    }
}

void HandleError::showError(const string &s)
{
  if (s.length() > 0)
    showError(s.c_str());
}
