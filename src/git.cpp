#include "git.hpp"
#include "constants.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include "toml.hpp"

// TODO: improve performance 
Git::Git() {
  std::vector<Unit> all_new, all_old;

  if (newConfigTbl.contains("git")) {
    fetch(newConfigTbl, all_new);
  }

  if (oldConfigTbl.contains("git")) {
    fetch(oldConfigTbl, all_old);
  }

  for (const Unit& u: all_new) {
    bool found = 0;

    for (const Unit &v : all_old) {
      found |= (v == u);
    }

    if (!found) {
      _additions.push_back(u);
    }
  }

  for (const Unit &u: all_old) {
    bool found = 0;

    for (const Unit &v: all_new) {
      found |= (u == v);
    }

    if (!found) {
      _deletions.push_back(u);
    }
  }
}

void Git::update() {
  for (const Unit &u : _deletions) {
    del(u);
  }

  for (const Unit& u: _additions) {
    add(u);
  }
}

void Git::add(const Unit &u) {
  for (const std::string& cmd: u._add_cmds) {
    utils::run(cmd.c_str());
  }

  for (const std::string &cmd : u._add_hooks) {
    utils::run(cmd.c_str());
  }
}

void Git::del(const Unit &u) {
  for (const std::string &cmd : u._del_hooks) {
    utils::run(cmd.c_str());
  }

  for (const std::string& cmd: u._del_cmds) {
    utils::run(cmd.c_str());
  }
}

void Git::fetch(const toml::table &tbl, std::vector<Unit> &result) {
  const char *add_cmd = tbl["git"]["add_cmd"].value_or("");
  const char *del_cmd = tbl["git"]["del_cmd"].value_or("");

  COPPER_LOG_ASSERT(strlen(add_cmd) != 0);
  COPPER_LOG_ASSERT(strlen(del_cmd) != 0);

  if (const toml::array* arr = tbl["git"]["list"].as_array()) {
    for (auto it = arr->begin(); it != arr->end(); ++it) {
      COPPER_LOG_ASSERT(it->is_table());


      const toml::table it_tbl = *it->as_table();

      const std::string alias = it_tbl["alias"].value_or("");
      const std::string repo_url = it_tbl["repo_url"].value_or("");
      const std::string clone_dir = it_tbl["clone_dir"].value_or("");

      Unit u(alias);

      std::string cmd = add_cmd;
      cmd.replace(cmd.find("#1"), 2, repo_url);
      cmd.replace(cmd.find("#2"), 2, clone_dir);
      u._add_cmds.push_back(cmd);

      cmd = del_cmd;
      cmd.replace(cmd.find("#1"), 2, clone_dir);
      u._del_cmds.push_back(cmd);

      if (const toml::array* add_cmds_arr = tbl["git"][alias]["add_cmds"].as_array()) {
        for (auto it = add_cmds_arr->begin(); it != add_cmds_arr->end(); ++it) {
          u._add_hooks.push_back(it->value_or(""));
        }
      }

      if (const toml::array* del_cmds_arr = tbl["git"][alias]["del_cmds"].as_array()) {
        for (auto it = del_cmds_arr->begin(); it != del_cmds_arr->end(); ++it) {
          u._del_hooks.push_back(it->value_or(""));
        }
      }

      result.push_back(u);
    }
  }
}
