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
    size_t sz = strlen(x);                                                     \
    for (size_t i = 0; i < sz; ++i)                                            \
      std::cerr << x[i];                                                       \
    std::cerr << '\n';                                                         \
  }

#define COPPER_LOG_INFO(x)                                                     \
  {                                                                            \
    std::cerr << greenColorCode << "SUCCESS: " << resetColorCode;              \
    size_t sz = strlen(x);                                                     \
    for (size_t i = 0; i < sz; ++i)                                            \
      std::cerr << x[i];                                                       \
    std::cerr << '\n';                                                         \
  }
