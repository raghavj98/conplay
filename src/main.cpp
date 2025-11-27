#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/color.hpp>
#include <vector>
#include <string>
#include <math.h>
#include <dlfcn.h>
#include <map>
#include <thread>
#include <chrono>
#include <mutex>

ftxui::Element getTile(ftxui::Color color, int width, int height) {
  return ftxui::text("") 
    | bgcolor(color)
    | size(ftxui::WIDTH, ftxui::EQUAL, width)
    | size(ftxui::HEIGHT, ftxui::EQUAL, height);
}

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

class ComponentSelector: public ftxui::ComponentBase {

  private:
  std::map<std::string, ftxui::Component> items;
  ftxui::Elements nameEntries;
  int selected = 0;
  ftxui::ElementDecorator menuDecorator = ftxui::borderRounded | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 20);
  ftxui::Component welcome = ftxui::Renderer([]{return getTile(ftxui::Color::White, 160, 60);});
  ftxui::Component *active = &welcome;
  bool inmenu = true;

  public:
  ComponentSelector(std::map<std::string, ftxui::Component> items) :
    ComponentBase(),
    items((items)) {

    for (const auto& item : items) {
      nameEntries.emplace_back(ftxui::text(">" + item.first));
    }
  }

  ftxui::Element OnRender() override {
    ftxui::Elements afterSelection = nameEntries;
    afterSelection.at(selected) |= ftxui::inverted;
    auto config = ftxui::FlexboxConfig();
    config.Set(ftxui::FlexboxConfig::JustifyContent::Center);
    auto menu = ftxui::vbox(afterSelection) | menuDecorator;
    return ftxui::window(
        ftxui::text("Conplay"),
        ftxui::hbox({
          menu, 
          ftxui::flexbox({(*active)->Render() | ftxui::border}, config) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 240)
        }));
  }

  bool OnEvent(ftxui::Event event) override {
    if (event == ftxui::Event::q) {
      active = &welcome;
      inmenu = true;
      return true;
    }
    if (!inmenu) {
      return (*active)->OnEvent(event);
    }
    if (event == ftxui::Event::Return) {
      inmenu = false;
      active = &(std::next(items.begin(), selected)->second);
      return true;
    }
    if (event == ftxui::Event::j) {
      (selected)++;
      if (selected >= nameEntries.size()) {
        selected = nameEntries.size() - 1;
      }
      return true;
    }
    if (event == ftxui::Event::k) {
      (selected)--;
      if (selected < 0) {
        selected = 0;
      }
      return true;
    }
    return false;
  }
};

int main() {
  auto screen = ftxui::ScreenInteractive::TerminalOutput();
    void *handle;
    ftxui::Component (*greet_func)(); // Function pointer to the 'greet' function
    std::string (*name_func)(); // Function pointer to the 'greet' function
    char *error;
    handle = dlopen("./libanimation.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    name_func = (std::string (*)()) dlsym(handle, "getName");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "dlsym failed: %s\n", error);
        dlclose(handle); // Close the library on error
        exit(EXIT_FAILURE);
    }
    auto name = name_func();

    // Get the address of the 'greet' function
    greet_func = (ftxui::Component (*)()) dlsym(handle, "getComp");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "dlsym failed: %s\n", error);
        dlclose(handle); // Close the library on error
        exit(EXIT_FAILURE);
    }

    auto dlComp = greet_func();
    auto comp = ftxui::Make<ComponentSelector, std::map<std::string, ftxui::Component>>({
      {"Red", ftxui::Renderer([]{return getTile(ftxui::Color::Red, 160, 60);})},
      {"Green", ftxui::Renderer([]{return getTile(ftxui::Color::Green, 160, 60);})},
      {"Blue", ftxui::Renderer([]{return getTile(ftxui::Color::Blue, 160, 60);})},
      {name , dlComp},
    });


    // Close the shared library
    screen.Loop(comp);
    // dlclose(handle);

  return 0;
}
