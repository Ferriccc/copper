#include <filesystem>
#include <format>
#include <regex>

#include "constants.hpp"
#include "logger.hpp"
#include "symlinks.hpp"
#include "toml.hpp"
#include "utils.hpp"

// TODO: improve performance
Symlinks::Symlinks() {
  if (newConfigTbl.contains("symlinks")) {
    fetch(newConfigTbl, _additions, 1);
  }

  if (oldConfigTbl.contains("symlinks")) {
    fetch(oldConfigTbl, _deletions, 0);
  }
}

namespace fs = std::filesystem;

void Symlinks::update() {
  for (const auto &[_, to] : _deletions) {
    del(to);
  }

  for (const auto &[from, to] : _additions) {
    if (fs::exists(to) || fs::is_symlink(to)) {
      del(to);
    }

    add(from, to);
  }
}

void Symlinks::add(const std::string &from, const std::string &to) {
  std::error_code ec;

  fs::create_directories(fs::path(to).parent_path(), ec);
  fs::create_symlink(from, to, ec);

  if (ec == std::errc::permission_denied) {
    COPPER_LOG_WARN(
        std::format("Elevation required for symlinking {} -> {}\nEnter 'y' for "
                    "elevating or 'n' to skip...",
                    from, to));

    char userResponse;
    std::cin >> userResponse;

    if (userResponse != 'y')
      return;

    utils::run(std::format("sudo ln -s {} {}", from, to).c_str());
  }
}

void Symlinks::del(const std::string &destination) {
  std::error_code ec;
  fs::remove(destination, ec);

  if (ec == std::errc::permission_denied) {
    COPPER_LOG_WARN(std::format(
        "Elevation required for removing exsisting file {}\nEnter 'y' for "
        "elevating or 'n' to skip...",
        destination));

    char userResponse;
    std::cin >> userResponse;

    if (userResponse != 'y')
      return;

    utils::run(std::format("sudo rm -rf {}", destination).c_str());
  }
}

void Symlinks::fetch(const toml::table &tbl,
                     std::vector<std::pair<std::string, std::string>> &result,
                     bool f) {
  std::string prefix = tbl["symlinks"]["prefix"].value_or("");

  if (prefix != "") {
    utils::sanitiseDirPathEnding(prefix);
    COPPER_LOG_ASSERT(fs::is_directory(prefix));
  }

  // TODO: improve performance
  std::vector<std::regex> exclusions;
  if (const toml::array *arr = tbl["symlinks"]["exclude"].as_array()) {
    arr->for_each([&](const auto &ele) {
      exclusions.push_back(std::regex(prefix + "/" + ele.value_or("")));
    });
  }

  auto isExcluded = [&](const std::string &destination) -> bool {
    for (const std::regex &pattern : exclusions) {
      if (std::regex_match(destination, pattern)) {
        return true;
      }
    }

    return false;
  };

  const std::string dir = (f ? newGenDir : oldGenDir);

  for (const auto &it : fs::recursive_directory_iterator(dir)) {
    if (!it.is_regular_file()) {
      continue;
    }

    const std::string from = it.path();
    const std::string to = utils::replacePrefix(from, dir, prefix);

    if (isExcluded(to)) {
      continue;
    }

    result.emplace_back(from, to);
  }
}
