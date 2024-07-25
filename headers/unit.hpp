#pragma once

#include <string>
#include <vector>

struct Unit {
  std::vector<std::string> _add_cmds, _del_cmds;
  std::vector<std::string> _add_hooks, _del_hooks;

  bool operator==(const Unit &u) const;
};
