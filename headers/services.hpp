#pragma once

#include <vector>

#include "toml.hpp"
#include "unit.hpp"

class Services {
public:
  Services();
  void update();

private:
  std::vector<Unit> _additions;
  std::vector<Unit> _deletions;

  void add(const Unit &u);
  void del(const Unit &u);

  void fetch(const toml::table &tbl, std::vector<Unit> &result);
};
