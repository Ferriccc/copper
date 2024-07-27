#include "unit.hpp"

Unit::Unit() : _alias("") {}

Unit::Unit(const std::string &alias) : _alias(alias) {}

bool Unit::operator==(const Unit &u) const {
  bool eq = 1;
  eq &= (_alias == u._alias);
  eq &= (_add_cmds == u._add_cmds);
  eq &= (_del_cmds == u._del_cmds);
  eq &= (_add_hooks == u._add_hooks);
  eq &= (_del_hooks == u._del_hooks);
  return eq;
}
