# Copper

**Copper** is a declarative way of managing Linux system configuration. It allows you to define your system configuration in a toml file and apply it consistently across your systems.

## Table of Contents

* [Features](#features)
* [Dependencies](#dependencies)
* [Installation](#installation)
* [Setup](#setup)
* [Usage](#usage)
* [Examples](#examples)
* [Updates](#updates)

## Features

- **Declarative Syntax**: Copper uses toml files for configuration declaration, unlike imperative stlye followed by most linux distros, this follows a declarative paradigm.
- **Consistency**: Because of it's declarative nature, one can replicate systems with just a single toml file.
- **Ease of Use**: Toml files are very similar to dictionaries with key-value pairs, making it very intuitive to use.
- **Safe**: Copper supports generation tracking, broke your system by installing / uninstalling some package?, no worries, copper can revert to previously working configuration.
- **Hook support**: Copper supports hooks to be ran after performing some changes in configuration, making it possible to avoid imperative style of commands even further.

## Dependencies

- [make](https://www.gnu.org/software/make/)
- [toml++](https://github.com/marzer/tomlplusplus): A great Library by [marzer](https://github.com/marzer) for parsing toml files in c++, this is pre-included and is **not** needed to be installed separately.

## Installation

To install Copper, follow these steps:

```bash
# Clone the repository
$ git clone https://github.com/ferriccc/copper.git
```

```bash
# cd into the cloned directory and install
$ cd copper && sudo make release
```

```bash
# install in debug mode, useful for reporting bugs
$ cd copper && sudo make test
```

## Setup
- Copper takes all it's configuration options from the config.toml file
- Start by making a directory for copper to store all it's generations in. (Recommended: ~/.copper)
- Next create a directory, and a file named **config.toml** in it with following contents:
```toml
[copper] # let copper manage it's own config!
generation_store_dir = "/path/to/generation_storage_directory"
max_generations = 5 # set to 0 for keeping all generations
```
- Now you are all set to start using copper. Refer to [usage](#usage) section for all possible use cases.

## Usage
```bash
# copper requires the only argument to be the directory of config.toml file which is to be applied
$ copper /path/to/directory/containing_config.toml_file
```
* Refer the respective sections for specific usage of each modules:
    * [System-Packages](#system-packages)
    * [Services](#services)
    * [Symlinks (Config files of other programs)](#symlinks)
    * [Git repos](#git)

## System-packages
* copper supports installing and uninstalling packages declaratively, to get started configure the package install / uninstall commands for your linux distro by adding following lines to config.toml file:
```toml
[system_packages] 
add_cmd = "sudo apt install #1" # "#1" is a placeholder to tell copper to replace it with package name
del_cmd = "sudo apt remove #1"
```
* Now you can start adding packages to your system by just adding them in the list by adding following lines:
```toml
list = [
    "make",
    "htop",
    "neovim",
    # ...
]
```
* Each time when a new generation is built, copper will sync packages to match the above defined list.
* Hooks can be added on **per-package** basis in following syntax:
```toml
[system_packages.visual-studio-code-bin]
add_cmds = [
  "sudo chown $(whoami) -R /opt/visual-studio-code/",
  # ...
] # add hooks will be ran after adding the respective package
del_cmds = [
  "sudo rm -rf /opt/visual-studio-code/",
  # ...
] # del hooks will be ran after respective package is removed.
```

## Services
* copper supports managing active / inactive systemd-services, start by adding following lines to your config.toml file:
```toml
[services]
add_cmd = "sudo systemctl enable --now #1" # "#1" is a placeholder to tell copper to replace it with service name
del_cmd = "sudo systemctl disable --now #1"
```
* Now you can defining services declartively by adding following lines:
```toml
list = [
    "bluetooth.service", 
    "powertop.service",
    # ...
]
```
* Each time when a new generation is build, copper will activate / deactivate services to match the above list.
* Hooks can be added on **per-service** basis in following syntax:
```toml
[services.bluetooth.service]
add_cmds = [
  # ...
] # add hooks will be ran after adding the respective package
del_cmds = [
  # ...
] # del hooks will be ran after respective package is removed.
```

## Symlinks
* With copper, you can store all the config files for other programs, (e.g. .bashrc) in the same structure as they are supposed to be under root (/), in the directory containing the config.toml file and copper will take care of symlinking them to their required location.
* For example:
``` bash
# /home/user/.bashrc => should be stored at /path/to/dir_of_config.toml/home/user/.bashrc
```
* This makes is very easy for version tracking, just make a git repo inside the directory containing config.toml to track every config file of programs used in your linux system.
* This also enables you to preserve changes, you can revert to some previous file content using copper's generation in case of editing a file breaks your system.
* To track files, which are not owned by current user, add following lines to config.toml:
```toml
[symlinks]
sudo_add_cmd = "sudo mkdir -p $(dirname #2) && sudo ln -s #1 #2"
sudo_del_cmd = "sudo rm -rf #1"
```
* Exclusion rules can be added, and supports **regular expressions**:
```toml
[symlinks]
sudo_add_cmd = "sudo mkdir -p $(dirname #2) && sudo ln -s #1 #2"
sudo_del_cmd = "sudo rm -rf #1"
exclude = ["config\\.toml", "\\.git.*"]
```
## Git
* Git module of copper deals with routines like cloning a git repository, and performing some installation / building commands as a hook after that.
* Start by defining clone / removal commands in config.toml:
```toml
[git]
add_cmd = "git clone #1 #2" # "#1", "#2" are placeholders, will be replaced by repo url, destination respectively
del_cmd = "rm -rf #1"
```
* Now you can start adding repositories by following syntax:
```toml
[[git.list]]
alias = "paru" # aliases are useful to refer the repo, for adding hooks in future
repo_url = "https://aur.archlinux.org/paru.git"
clone_dir = "/home/user/paru"
```
* Hooks can be added in similar syntax to previous modules:
```toml
[git.paru] # refer the repo with it's alias
add_cmds = ["(cd /home/shrey/paru/ && makepkg -si)"]
del_cmds = ["sudo pacman -R paru"]
```

## Examples
- Take a look at my configuration at my [Linux-configs](https://github.com/Ferriccc/my-linux-configs) for example use.

## Updates
- ~I am planning to add support for rollbacks so that if something breaks you can rollback to previous state of the system using copper~ (UPD: added!)
- Any suggestions / Feature requests are more than welcomed.
