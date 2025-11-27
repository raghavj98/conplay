#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/color.hpp>
#include "vec.h"


ftxui::Element getTile1(ftxui::Color color, int width, int height) {
  return ftxui::text("") 
    | bgcolor(color)
    | size(ftxui::WIDTH, ftxui::EQUAL, width)
    | size(ftxui::HEIGHT, ftxui::EQUAL, height);
}



class Animated : public ftxui::ComponentBase {
  const int height = 60;
  const int width = 160;
  int count = 0;
  ftxui::Element currFrame = getTile1(ftxui::Color::Red, width, height);


  ftxui::Element OnRender() override {
    updateFrame();
    ftxui::animation::RequestAnimationFrame();
    return currFrame;
  }

  void updateFrame() {
    count++;
    auto elems = helper(count);
    ftxui::Elements tmp;
    tmp.reserve(elems.size());
    for (const auto& row: elems) {
      tmp.push_back(ftxui::hbox(row));
    }
    currFrame = ftxui::vbox(tmp);
  }

  std::vector<ftxui::Elements> helper(int frame) {
    std::vector<ftxui::Elements> ret;
    ret.reserve(height);
    float t = (frame % 240) / 240.0f * 2.0f * M_PI;
    vec2 r = {(float)width, (float)height};

    for (int y = 0; y < height; ++y) {
      ftxui::Elements row;
      row.reserve(width);

      for (int x = 0; x < width; ++x) {
        //------------------------------------
        // Plasma shader math (converted to C++)
        //------------------------------------
        vec4 o;
        vec2 FC = {(float)x, (float)y};

        vec2 p = (FC * 2.f - r) / r.y;
        vec2 l;
        vec2 i; 
        vec2 v = p * (l += 4.f - 4.f * abs(0.7f - dot(p,p)));

        for (; i.y++ < 8.f; o += (sin(v.xyyx()) + 1.f) * fabsf(v.x - v.y)) {
            v += cos(v.yx() * i.y + i + t) / i.y + 0.7f;
        }

        o = tanh(5.f * exp(l.x - 4.f - p.y * vec4(-1,1,2,0)) / o);

        //------------------------------------
        // Convert plasma (0..1 floats) to 8-bit RGB
        //------------------------------------
        int R = std::clamp((int)(o.x * 255), 0, 255);
        int G = std::clamp((int)(o.y * 255), 0, 255);
        int B = std::clamp((int)(o.z * 255), 0, 255);

        //------------------------------------
        // Draw pixel: use “ ” with background color
        //------------------------------------
        row.push_back(
            ftxui::text(" ") | ftxui::bgcolor(ftxui::Color::RGB(R,G,B))
        );
      }
      ret.push_back(std::move(row));
    }
    return ret;
  }
};

extern "C" {
std::string getName() {
    return "Animated";
}

ftxui::Component getComp() {
    return ftxui::Make<Animated>();
}
}
