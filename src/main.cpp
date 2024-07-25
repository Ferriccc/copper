#include <filesystem>

#include "constants.hpp"
#include "system_packages.hpp"
#include "logger.hpp"

void apply(const std::string& path) {
  namespace fs = std::filesystem;
  fs::copy(path, newGenDir,
           (fs::copy_options::update_existing | fs::copy_options::recursive));

  SystemPackages sp;
  sp.update();

  infoTbl.insert_or_assign("current", newGenDir);
  std::ofstream outfile(genStoreDir + "/info.toml");
  outfile << infoTbl;
  outfile.close();
}

int main(int argc, char **argv) {
  COPPER_LOG_ASSERT(argc == 2);

  std::string path = argv[1];

  constants::populate(path);

  apply(path);
}
