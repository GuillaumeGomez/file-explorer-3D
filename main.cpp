#include <iostream>
#include <exception>
#include <cstdlib>
#include "MyWindow.hpp"
#include "HandleError.hpp"

using namespace std;

int main(int argc, char **argv)
{
  try {
    MyWindow  w("My world");

    if (argc == 2) {
        w.connectToServer(argv[1]);
    } else {
        w.startServers();
    }
    (void)argc;
    (void)argv;
    w.start();
  } catch (std::exception &e){
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
