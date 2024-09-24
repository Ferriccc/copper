#pragma once

#include "toml.hpp"

extern std::string genStoreDir;
extern std::string newGenDir, oldGenDir;
extern toml::table newConfigTbl, oldConfigTbl, infoTbl;
extern int maxGens;

namespace constants {

void populate(std::string &path);

} // namespace constants
