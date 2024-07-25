#include <cassert>
#include <chrono>
#include <cstdlib>
#include <random>
#include <unistd.h>

#include "logger.hpp"
#include "utils.hpp"

namespace utils {

void sanitiseDirPathEnding(std::string &path) {
  COPPER_LOG_ASSERT(path.size() > 0);
  if (path.back() == '/') {
    path.pop_back();
    COPPER_LOG_WARN("found '/' at provided path ending");
  }
}

void run(const char *cmd) {
#ifndef TESTING
  int ret = system(cmd);
  COPPER_LOG_ASSERT(ret != -1 && WIFEXITED(ret) && WEXITSTATUS(ret) == 0);
#endif
  COPPER_LOG_INFO(cmd);
}

std::string replacePrefix(const std::string&  target, const std::string &from, const std::string &to) {
  COPPER_LOG_ASSERT(target.size() > 0 && target.starts_with(from));

  std::string res = to + target.substr(from.size());

  return res;
}

std::string randomString(const int length) {
  std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());

  const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

  std::uniform_int_distribution<> distribution(0, characters.size() - 1);

  std::string res = "";
  res.reserve(length);
  for (int i = 0; i < length; ++i) {
    res += characters[distribution(rng)];
  }

  return res;
}

} // namespace utils
