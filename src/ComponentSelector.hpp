#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <map>

ftxui::Element getTile(ftxui::Color color, int width, int height) {
  return ftxui::text("") 
    | bgcolor(color)
    | size(ftxui::WIDTH, ftxui::EQUAL, width)
    | size(ftxui::HEIGHT, ftxui::EQUAL, height);
}

class ComponentSelector: public ftxui::ComponentBase {

  private:
  std::map<std::string, ftxui::Component> items;
  ftxui::Elements nameEntries;
  int selected = 0;
  ftxui::ElementDecorator menuDecorator = ftxui::borderRounded 
                                          | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 30);
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
    auto menu = ftxui::vbox({
        ftxui::vbox(afterSelection),
        ftxui::filler(),
        ftxui::separator(),
        ftxui::hbox({ftxui::text("Quit (q)"), ftxui::filler(), ftxui::separator(), ftxui::filler(), ftxui::text("Select (Enter)")})
    }) | menuDecorator;
    return ftxui::window(
        ftxui::text("Conplay"),
        ftxui::hbox({
          menu, 
          ftxui::flexbox({(*active)->Render()
              | ftxui::border}, config)
              | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 240)
        }));
  }

  bool OnEvent(ftxui::Event event) override {
    if (event == ftxui::Event::q) {
      active = &welcome;
      if (inmenu) {
        ftxui::ScreenInteractive* active_screen = ftxui::ScreenInteractive::Active();
        if (active_screen) {
          active_screen->ExitLoopClosure()();
        }
      }
      else {
        inmenu = true;
      }
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

