#include <filesystem>
#include <regex>
#include <iostream>

#include "symlinks.hpp"
#include "constants.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include "toml.hpp"

// TODO: improve performance things are very slow here
Symlinks::Symlinks() {
  if (newConfigTbl.contains("symlinks")) {
    fetch(newConfigTbl, _additions);
  }

  if (oldConfigTbl.contains("symlinks")) {
    fetch(oldConfigTbl, _deletions);
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
    // TODO: add option for linking to non permitted directories using "sudo"
  }
}

void Symlinks::del(const std::string &destination) {
  std::error_code ec;
  fs::remove(destination, ec);

  if (ec == std::errc::permission_denied) {
    // TODO: add option for linking to non permitted directories using "sudo"
  }
}

void Symlinks::fetch(const toml::table &tbl, std::vector<std::pair<std::string, std::string>> &result) {
  std::string prefix = tbl["symlinks"]["prefix"].value_or("");

  if (prefix != "") {
    utils::sanitiseDirPathEnding(prefix);
    COPPER_LOG_ASSERT(fs::is_directory(prefix));
  }

  // TODO: improve performance
  std::vector<std::regex> exclusions;
  if (const toml::array* arr = tbl["symlinks"]["exclude"].as_array()) {
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

  const std::string dir = (tbl == newConfigTbl ? newGenDir : oldGenDir);
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
