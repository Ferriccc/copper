#include <algorithm>
#include <filesystem>
#include <fstream>

#define TOML_IMPLEMENTATION
#include "constants.hpp"
#include "git.hpp"
#include "logger.hpp"
#include "services.hpp"
#include "symlinks.hpp"
#include "system_packages.hpp"

void apply(const std::string &path) {
  namespace fs = std::filesystem;
  fs::copy(path, newGenDir,
           (fs::copy_options::update_existing | fs::copy_options::recursive));

  Git gt;
  Symlinks sl;
  SystemPackages sp;
  Services sr;

  gt.update();
  sl.update();
  sp.update();
  sr.update();

  infoTbl.insert_or_assign("current", newGenDir);
  std::ofstream outfile(genStoreDir + "/info.toml");
  outfile << infoTbl;
  outfile.close();

  std::vector<fs::directory_entry> allGens;
  for (auto &it : fs::directory_iterator(genStoreDir)) {
    if (!fs::is_directory(it))
      continue;
    allGens.push_back(it);
  }

  if (allGens.empty() || maxGens == 0 || allGens.size() <= maxGens)
    return;

  std::sort(allGens.begin(), allGens.end(), [&](auto &x, auto &y) {
    return fs::last_write_time(x) > fs::last_write_time(y);
  });

  for (int i = maxGens; i < allGens.size(); ++i) {
    fs::remove_all(allGens[i]);
  }

  COPPER_LOG_INFO("Cleaned older generations");
}

int main(int argc, char **argv) {
  COPPER_LOG_ASSERT(argc == 2);

  std::string path = argv[1];

  constants::populate(path);

  apply(path);
}
