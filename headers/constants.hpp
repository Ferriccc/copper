#pragma once

#include "toml.hpp"

extern std::string genStoreDir;
extern std::string newGenDir;
extern toml::table newConfigTbl, oldConfigTbl, infoTbl;

namespace constants {

void populate(std::string &path);

} // namespace constants
