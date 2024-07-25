#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std::string_view_literals;
namespace fs = std::filesystem;

const std::string GENSTOREDIR = "/home/shrey/copper/tests/.copper";
const std::string CONFIGDIR = "/home/shrey/copper/tests/dir";
const std::string SRCDIR = "/home/shrey/copper";
const std::string COMPILECMD = "make all";
const std::string OUT = "/home/shrey/copper/tests/out.txt";
const std::string OUTSUFFIX = " 2> " + OUT;

#define ASSERT(x)                                                              \
  if (!(x)) {                                                                  \
    std::cerr << "\033[31mASSERTION FAILED: " << #x << "\033[0m" << std::endl; \
    exit(1);                                                                   \
  } else {                                                                     \
    std::cerr << "\033[32mASSERTION PASSED: " << #x << "\033[0m" << std::endl; \
  }

void compile() {
  int ret = system("(cd /home/shrey/copper/ && make all)");
  ASSERT(ret != -1 && WIFEXITED(ret) && WEXITSTATUS(ret) == 0);
}

void writeToTestConfig(const std::basic_string_view<char> &source) {
  fs::create_directories(CONFIGDIR);
  std::ofstream of(CONFIGDIR + "/config.toml");
  of << source << std::endl;
  of.close();
}

void writeToTestConfigOld(const std::basic_string_view<char> &source, const std::string &gen_name) {
  std::string path = GENSTOREDIR + "/" + gen_name;
  fs::create_directories(path);

  std::ofstream of(path + "/config.toml");
  of << source << std::endl;
  of.close();

}

void writeToInfo(const std::basic_string_view<char> &source) {
  fs::create_directories(GENSTOREDIR);
  std::ofstream of(GENSTOREDIR + "/info.toml");
  of <<  source << std::endl;
  of.close();
}

void cleanDirs() {
    std::error_code ec;    
    fs::remove_all(GENSTOREDIR, ec);
    fs::remove_all(CONFIGDIR, ec);
}

void verifyOutput(const std::vector<std::string> &&exp, bool can_continue = 0) {
    int i = 0;
    std::ifstream outfile(OUT);
    std::string line;
    while (getline(outfile, line)) {
      if (!can_continue) {
        ASSERT(i < exp.size());
      } else if (i >= exp.size()) {
        break;
      }
      ASSERT(line == exp[i]);
      ++i;
    }
}

void testMain() {
  { // argc test with empty arg
    cleanDirs();
    system(std::string("copper" + OUTSUFFIX).c_str());
    verifyOutput({
      "ASSERTION FAILED: argc == 2"
    });
  }
  ASSERT("PASSED testMain 1");

  { // argc test with more args than needed
    cleanDirs();
    system(std::string("copper arg1 arg2 arg10" + OUTSUFFIX).c_str());
    verifyOutput({
      "ASSERTION FAILED: argc == 2"
    });
  } 
  ASSERT("PASSED testMain 2");
}

void testConstants() {
  { // providing path with ending '/'
    cleanDirs();
    system(std::string("copper /home/shrey/copper/tests/dir/" + OUTSUFFIX).c_str());
    verifyOutput({
      "WARN: found '/' at provided path ending"
    }, 1);
  }
  ASSERT("PASSED testConstants 1");

  { // providing non-exsistent path
    cleanDirs();
    system(std::string("copper /someNonExsistentPath" + OUTSUFFIX).c_str());
    verifyOutput({
      "ASSERTION FAILED: std::filesystem::exists(new_config_path)"
    });
  }
  ASSERT("PASSED testConstants 2");

  { // providing empty generation_store_dir in config.toml
    cleanDirs();
    writeToTestConfig(R"(
[copper]
    )"sv);
    system(std::string("copper /home/shrey/copper/tests/dir" + OUTSUFFIX).c_str());
    verifyOutput({
      "ASSERTION FAILED: genStoreDir != \"\""
    });
  }
  ASSERT("PASSED testConstants 3");

  { // providing generation_store_dir with ending '/'
    cleanDirs();
    writeToTestConfig(R"(
[copper]
generation_store_dir = "/home/shrey/copper/tests/.copper/"
    )"sv);
    system(std::string("copper /home/shrey/copper/tests/dir" + OUTSUFFIX).c_str());
    verifyOutput({
      "WARN: found '/' at provided path ending"
    }, 1);
  }
  ASSERT("PASSED testConstants 4");

  { // providing non-exsistent generation_store_dir AND info.toml
    cleanDirs();
    writeToTestConfig(R"(
[copper]
generation_store_dir = "/home/shrey/copper/tests/.copper"
    )"sv);
    system(std::string("copper /home/shrey/copper/tests/dir" + OUTSUFFIX).c_str());
    verifyOutput({
      "WARN: generation_store_dir path defined in config.toml does not exists, creating ...",
      "WARN: info.toml file does not exists, creating ..."
    }, 1);
    ASSERT(fs::exists(GENSTOREDIR));
    ASSERT(fs::exists(GENSTOREDIR + "/info.toml"));
  }
  ASSERT("PASSED testConstants 5");
}

void testSystemPackages() {
  { // no system_packages table
    cleanDirs();
    writeToInfo("");
    writeToTestConfig(R"(
[copper]
generation_store_dir = "/home/shrey/copper/tests/.copper"
    )"sv);
    system(std::string("copper /home/shrey/copper/tests/dir" + OUTSUFFIX).c_str());
    verifyOutput({
      "WARN: new config.toml does not contain \"system_packages\" table, ignoring ...",
      "WARN: old config.toml does not contain \"system_packages\" table, ignoring ...",
    });
  }
  ASSERT("PASSED testSystemPackages 1");

  { // empty add / del cmd
    cleanDirs();
    writeToInfo("");
    writeToTestConfig(R"(
[copper]
generation_store_dir = "/home/shrey/copper/tests/.copper"

[system_packages]
    )"sv);
    system(std::string("copper /home/shrey/copper/tests/dir" + OUTSUFFIX).c_str());
    verifyOutput({
      "ASSERTION FAILED: strlen(add_cmd) != 0",
    });
  }
  ASSERT("PASSED testSystemPackages 2");

  { // only new config and no old
    cleanDirs();
    writeToTestConfigOld(R"(
[copper]
generation_store_dir = "/home/shrey/copper/tests/.copper"

[system_packages]
add_cmd = "ADD_PACK #1"
del_cmd = "DEL_PACK #1"
                         )"sv,
                         "gen1");
    writeToInfo(R"(
current = "/home/shrey/copper/tests/.copper/gen1"
                )"sv);
    writeToTestConfig(R"(
[copper]
generation_store_dir = "/home/shrey/copper/tests/.copper"

[system_packages]
add_cmd = "ADD_PACK #1"
del_cmd = "DEL_PACK #1"
list = [
  "PACKAGE1",
]
[system_packages.PACKAGE1]
add_cmds = [
  "ADD_PACK_HOOK1 PACKAGE1",
  "ADD_PACK_HOOK2 PACKAGE1",
]
    )"sv);
    system(std::string("copper /home/shrey/copper/tests/dir" + OUTSUFFIX).c_str());
    verifyOutput({
      "SUCCESS: ADD_PACK PACKAGE1",
      "SUCCESS: ADD_PACK_HOOK1 PACKAGE1",
      "SUCCESS: ADD_PACK_HOOK2 PACKAGE1"
    });
  }
  ASSERT("PASSED testSystemPackages 3");

  { // both old and new config
    cleanDirs();
    writeToTestConfigOld(R"(
[copper]
generation_store_dir = "/home/shrey/copper/tests/.copper"

[system_packages]
add_cmd = "ADD_PACK #1"
del_cmd = "DEL_PACK #1"
list = [
  "PACKAGE1"
]
[system_packages.PACKAGE1]
del_cmds = [
  "DEL_PACK_HOOK1 PACKAGE1",
  "DEL_PACK_HOOK2 PACKAGE1",
]
                         )"sv,
                         "gen2");
    writeToInfo(R"(
current = "/home/shrey/copper/tests/.copper/gen2"
                )"sv);
    writeToTestConfig(R"(
[copper]
generation_store_dir = "/home/shrey/copper/tests/.copper"

[system_packages]
add_cmd = "ADD_PACK #1"
del_cmd = "DEL_PACK #1"
list = [
  "PACKAGE2",
]
[system_packages.PACKAGE2]
add_cmds = [
  "ADD_PACK_HOOK1 PACKAGE2",
  "ADD_PACK_HOOK2 PACKAGE2",
]
    )"sv);
    system(std::string("copper /home/shrey/copper/tests/dir" + OUTSUFFIX).c_str());
    verifyOutput({
      "SUCCESS: DEL_PACK PACKAGE1",
      "SUCCESS: DEL_PACK_HOOK1 PACKAGE1",
      "SUCCESS: DEL_PACK_HOOK2 PACKAGE1",
      "SUCCESS: ADD_PACK PACKAGE2",
      "SUCCESS: ADD_PACK_HOOK1 PACKAGE2",
      "SUCCESS: ADD_PACK_HOOK2 PACKAGE2",
    });
  }
  ASSERT("PASSED testSystemPackages 4");
}

int main() {
  compile();

  testMain();
  testConstants();
  testSystemPackages();

  cleanDirs();
}
