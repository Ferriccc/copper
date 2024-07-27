#include "/home/shrey/copper/external_deps/toml.hpp"
#include <filesystem>
#include <iostream>

const char *redColorCode = "\e[1;31m";
const char *greenColorCode = "\e[1;32m";
const char *yellowColorCode = "\e[1;33m";
const char *resetColorCode = "\e[0m";

#define ASSERT(x)                                                                         \
  if (!(x)) {                                                                             \
    std::cerr << redColorCode << "CHECK FAILED: " << resetColorCode << #x << std::endl;   \
    exit(1);                                                                              \
  }

#define SUCCESS(x) std::cerr << greenColorCode << "WORKS: " << resetColorCode << x << std::endl;

#define DBG(x) std::cerr << yellowColorCode << "DBG: " << resetColorCode << x << std::endl;

namespace fs = std::filesystem;

const std::string OUT = "/home/shrey/copper/tests/out.txt";
const std::string CMDSUFFIX = " 2> " + OUT;
const std::string TEMPLATE_DIR = "/home/shrey/copper/tests/containers_template";
const std::string TESTS_DIR = "/home/shrey/copper/tests/containers";

void compile() {
  int ret = system("(cd /home/shrey/copper/ && make clean test)");
  ASSERT(ret != -1 && WIFEXITED(ret) && WEXITSTATUS(ret) == 0);
  SUCCESS("it compiles");
}

void verifyFiles(const toml::table &tbl) {
  if (const toml::array *arr = tbl["exp_files"].as_array()) {
    arr->for_each([&](const auto &o) {
      ASSERT(fs::exists(o.value_or(""))); 
    });
  }
  DBG("verified exp_files");
}

void verifySymlinks(const toml::table &tbl) {
  if (const toml::array *arr = tbl["exp_symlinks"].as_array()) {
    arr->for_each([&](const auto &o) { 
      ASSERT(fs::is_symlink(o.value_or(""))); 
    });
  }
  DBG("verified exp_symlinks");
}

void verifyOutput(const toml::table &tbl) {
  std::ifstream outfile(OUT);
  std::string line;
  bool canContinue = tbl["continue"].value_or(0);

  if (const toml::array *arr = tbl["exp"].as_array()) {
    arr->for_each([&](const auto &o) {
      ASSERT(getline(outfile, line));
      ASSERT(line == o.value_or(""));
    });
  }

  if (!canContinue) {
    ASSERT(!getline(outfile, line));
  }

  outfile.close();

  DBG("verified exp")
}

void tester(std::string &path) {
  const toml::table &tbl = toml::parse_file(path + "/test.toml");

  const std::string name = tbl["name"].value_or("");

  std::string run_cmd = "copper";
  if (const toml::array* arr = tbl["args"].as_array()) {
    arr->for_each([&](const auto &arg) {
      run_cmd += " " + std::string(arg.value_or(""));
    });
  }

  run_cmd += CMDSUFFIX;

  int ret = system(run_cmd.c_str());

  verifyOutput(tbl);
  verifyFiles(tbl);
  verifySymlinks(tbl);

  SUCCESS(name);
}

int main(int argc, char** argv) {
  std::error_code ec;
  fs::remove_all(TESTS_DIR, ec);
  fs::copy(TEMPLATE_DIR, TESTS_DIR, fs::copy_options::recursive);

  compile();

  int cnt = std::stoi(argv[1]);

  for (int i = 1; i <= cnt; ++i) {
    std::string path = TESTS_DIR + "/c" + std::to_string(i);
    tester(path);
  }
}
