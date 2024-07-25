#pragma once

#include <string>

namespace utils {

void sanitiseDirPathEnding(std::string &path);

void run(const char *cmd);

std::string replacePrefix(const std::string&  target, const std::string &from, const std::string &to);

std::string randomString(const int length);

} // namespace utils
