#include <filesystem>
#include <fstream>

#include "constants.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include "toml.hpp"

std::string genStoreDir;
std::string newGenDir, oldGenDir;
toml::table newConfigTbl, oldConfigTbl, infoTbl;

namespace constants {

void populate(std::string &path) {
  utils::sanitiseDirPathEnding(path);

  const std::string new_config_path = path + "/config.toml";

  COPPER_LOG_ASSERT(std::filesystem::exists(new_config_path));

  newConfigTbl = toml::parse_file(new_config_path);

  genStoreDir = newConfigTbl["copper"]["generation_store_dir"].value_or("");

  COPPER_LOG_ASSERT(genStoreDir != "");

  utils::sanitiseDirPathEnding(genStoreDir);

  if (!std::filesystem::exists(genStoreDir)) {
    COPPER_LOG_WARN("generation_store_dir path defined in config.toml does not exists, creating ...");
    std::filesystem::create_directories(genStoreDir);
  }

  std::string info_file_path = genStoreDir + "/info.toml";

  if (!std::filesystem::exists(info_file_path)) {
    COPPER_LOG_WARN("info.toml file does not exists, creating ...");
    std::ofstream outfile(info_file_path);
    outfile.close();
  }

  infoTbl = toml::parse_file(info_file_path);

  newGenDir = genStoreDir + "/" + utils::randomString(20);

  oldGenDir = infoTbl["current"].value_or("");

  if (oldGenDir != "") {
    oldConfigTbl = toml::parse_file(oldGenDir + "/config.toml");
  }
}

} // namespace constants
