#include "logger.hpp"

#ifndef TESTING
const char *redColorCode = "\e[1;31m";
const char *greenColorCode = "\e[1;32m";
const char *yellowColorCode = "\e[1;33m";
const char *resetColorCode = "\e[0m";
#else
const char *redColorCode = "";
const char *greenColorCode = "";
const char *yellowColorCode = "";
const char *resetColorCode = "";
#endif
