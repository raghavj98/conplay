#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/color.hpp>

#include <vector>
#include <string>
#include <dlfcn.h>
#include <iostream>
#include <filesystem>
#include <map>

#include "ComponentSelector.hpp"

namespace fs = std::filesystem;

ftxui::Element getText() {
  std::stringstream ss("too write these which get this large day number still through there very never with than people eye for  also part than one real come play work course child end out become only hand must open but should place  how few long need system state interest face and off some number those general interest few little if own");

  auto config = ftxui::FlexboxConfig();
  config.SetGap(1,0);
  config.Set(ftxui::FlexboxConfig::JustifyContent::Center);

  ftxui::Elements elems;
  std::string word;
  while (getline(ss, word, ' ')) {
    ftxui::Elements chars;
    for (char& c: word) {
      chars.push_back(ftxui::text(std::string(1, c)));
    }
    elems.push_back(hbox(chars));
  }
  return ftxui::flexbox(elems, config) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 160);
}

std::vector<fs::path> readRoms(const fs::path& rom_dir) {
  if (!std::filesystem::exists(rom_dir) || !std::filesystem::is_directory(rom_dir)) {
    std::cout << "Given roms dir " << rom_dir << " does not exist" << std::endl;
  }
  std::vector<fs::path> ret;
  for (const auto& fname : fs::directory_iterator(rom_dir)) {
    if (fs::is_regular_file(fname.status())) {
      ret.push_back(fs::absolute(fname.path()));
    }
  }
  return ret;
}

std::pair<std::string, ftxui::Component> loadRom(std::string path_to_rom) {
  void *handle;
  ftxui::Component (*c_getComponent)();
  std::string (*c_getName)();
  char *error;
  handle = dlopen("./libanimation.so", RTLD_LAZY);
  if (!handle) {
      fprintf(stderr, "dlopen failed: %s\n", dlerror());
      exit(EXIT_FAILURE);
  }

  c_getName = (std::string (*)()) dlsym(handle, "getName");
  if ((error = dlerror()) != NULL) {
      fprintf(stderr, "dlsym failed: %s\n", error);
      dlclose(handle);
      exit(EXIT_FAILURE);
  }

  c_getComponent = (ftxui::Component (*)()) dlsym(handle, "getComp");
  if ((error = dlerror()) != NULL) {
      fprintf(stderr, "dlsym failed: %s\n", error);
      dlclose(handle);
      exit(EXIT_FAILURE);
  }

  return {c_getName(), c_getComponent()};
  
}

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: `conplay ./roms_dir`" << std::endl;
    return 1;
  }
  auto roms = readRoms(argv[1]);
  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  std::map<std::string, ftxui::Component> ui;

  for (const auto& path : roms) {
    auto rom = loadRom(path);
    ui[rom.first] = rom.second;
  }

  ui["Red"] = ftxui::Renderer([]{return getTile(ftxui::Color::Red, 160, 60);});
  ui["Green"] = ftxui::Renderer([]{return getTile(ftxui::Color::Green, 160, 60);});
  ui["Blue"] = ftxui::Renderer([]{return getTile(ftxui::Color::Blue, 160, 60);});

  auto comp = std::make_shared<ComponentSelector>(ui);
  screen.Loop(comp);
  return 0;
}
