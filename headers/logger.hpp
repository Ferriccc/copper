#pragma once

#include <cstring>
#include <iostream>

extern const char *redColorCode;
extern const char *resetColorCode;
extern const char *greenColorCode;
extern const char *yellowColorCode;

#define COPPER_LOG_ASSERT(x)                                                   \
  if (!(x)) {                                                                  \
    std::cerr << redColorCode << "ASSERTION FAILED: " << resetColorCode;       \
    std::cerr << #x << '\n';                                                   \
    exit(1);                                                                   \
  }

#define COPPER_LOG_WARN(x)                                                     \
  {                                                                            \
    std::cerr << yellowColorCode << "WARN: " << resetColorCode;                \
    std::cerr << x << '\n';                                                    \
  }

#define COPPER_LOG_INFO(x)                                                     \
  {                                                                            \
    std::cerr << greenColorCode << "SUCCESS: " << resetColorCode;              \
    std::cerr << x << '\n';                                                    \
  }
