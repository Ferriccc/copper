#include "unit.hpp"

bool Unit::operator==(const Unit &u) const {
  bool eq = 1;
  eq &= (_add_cmds == u._add_cmds);
  eq &= (_del_cmds == u._del_cmds);
  eq &= (_add_hooks == u._add_hooks);
  eq &= (_del_hooks == u._del_hooks);
  return eq;
}
