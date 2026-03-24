cmake_minimum_required(VERSION 3.10)
project(App)

set(CMAKE_CXX_STANDARD 20)

find_package(ftxui REQUIRED)

add_executable(app
    src/AppState.cpp
    src/main.cpp
    src/UIViews.cpp
    lib/tinyexpr.c
)

target_compile_options(app PRIVATE
    -O2 -Wall -Wextra
)

target_include_directories(app PRIVATE
    include
    lib
)

target_compile_options(app PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wno-array-bounds>
)

target_link_libraries(app PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)
