#include <filesystem>
#include <fstream>

#define TOML_IMPLEMENTATION
#include "constants.hpp"
#include "system_packages.hpp"
#include "services.hpp"
#include "git.hpp"
#include "logger.hpp"

void apply(const std::string& path) {
  namespace fs = std::filesystem;
  fs::copy(path, newGenDir,
           (fs::copy_options::update_existing | fs::copy_options::recursive));

  SystemPackages sp;
  Services sr;
  Git gt;

  sp.update();
  sr.update();
  gt.update();

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
