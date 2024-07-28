#pragma once

#include <vector>
#include <string>

#include "toml.hpp"

class Symlinks {
public:
  Symlinks();
  void update();

private:
  std::vector<std::pair<std::string, std::string>> _additions;
  std::vector<std::pair<std::string, std::string>> _deletions;

  void add(const std::string &from, const std::string &to);
  void del(const std::string &destination);

  void fetch(const toml::table &tbl, std::vector<std::pair<std::string, std::string>> &result);
};
