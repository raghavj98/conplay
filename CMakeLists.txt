project (conplay)
set (SOURCES "src/main.cpp")
add_subdirectory(ext/ftxui)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME}
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)

# Rom should be dynamically linked .so
add_library("animation" SHARED "roms/animation/animation.cpp")
target_link_libraries("animation"
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
