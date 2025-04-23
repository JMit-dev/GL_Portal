#include "app/App.h"
#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
  try {
    App &app = App::getInstance();
    app.run();
  } catch (...) {
    throw;
  }
  return EXIT_SUCCESS;
}
