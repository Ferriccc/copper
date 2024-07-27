#pragma once

#include <string>
#include <vector>

struct Unit {
  std::string _alias;
  std::vector<std::string> _add_cmds, _del_cmds;
  std::vector<std::string> _add_hooks, _del_hooks;

  Unit();
  Unit(const std::string &alias);
  bool operator==(const Unit &u) const;
};
