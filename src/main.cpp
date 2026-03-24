#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "UIViews.hpp"

using namespace ftxui;

int main()
{
    auto screen = ScreenInteractive::Fullscreen();

    AppState state{};

    auto calcScreen = DrawUI(state);

    screen.Loop(calcScreen);

    return 0;
}
