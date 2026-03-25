#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "UIViews.hpp"
#include "KeyboardEvents.hpp"

using namespace ftxui;

int main()
{
    auto screen = ScreenInteractive::Fullscreen();

    AppState state{};

    auto calcScreen = DrawUI(state);

    auto eventHandler = GetEventCatcher(state);

    screen.Loop(calcScreen | eventHandler);

    return 0;
}
