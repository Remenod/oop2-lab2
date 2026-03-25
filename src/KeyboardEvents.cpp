#include "KeyboardEvents.hpp"
#include <ftxui/component/screen_interactive.hpp>
#include <vector>
#include <utility>

using namespace ftxui;

ftxui::ComponentDecorator GetEventCatcher(AppState &state)
{
    return CatchEvent(
        [&](Event event)
        {
            static const std::vector<std::pair<Event, ButtonAction>> key_bindings = {
                {Event::Character('p'), ButtonAction::Pi},
                {Event::Character('e'), ButtonAction::E},
                {Event::Character('('), ButtonAction::LParen},
                {Event::Character(')'), ButtonAction::RParen},

                {Event::Backspace, ButtonAction::ClearEntry},
                {Event::Character('='), ButtonAction::Equals},
                {Event::Return, ButtonAction::Equals},

                {Event::Character('l'), ButtonAction::Ln},
                {Event::Character('g'), ButtonAction::Log},
                {Event::Character('q'), ButtonAction::Sqrt},
                {Event::Character('s'), ButtonAction::Sin},
                {Event::Character('c'), ButtonAction::Cos},
                {Event::Character('t'), ButtonAction::Tan},

                {Event::Character('0'), ButtonAction::Digit0},
                {Event::Character('1'), ButtonAction::Digit1},
                {Event::Character('2'), ButtonAction::Digit2},
                {Event::Character('3'), ButtonAction::Digit3},
                {Event::Character('4'), ButtonAction::Digit4},
                {Event::Character('5'), ButtonAction::Digit5},
                {Event::Character('6'), ButtonAction::Digit6},
                {Event::Character('7'), ButtonAction::Digit7},
                {Event::Character('8'), ButtonAction::Digit8},
                {Event::Character('9'), ButtonAction::Digit9},
                {Event::Character('.'), ButtonAction::Dot},

                {Event::Character('+'), ButtonAction::Add},
                {Event::Character('-'), ButtonAction::Sub},
                {Event::Character('*'), ButtonAction::Mul},
                {Event::Character('/'), ButtonAction::Div},
                {Event::Character('^'), ButtonAction::Pow},
                {Event::Character('%'), ButtonAction::Percent},
            };

            for (const auto &[mapped_event, action] : key_bindings)
            {
                if (event == mapped_event)
                {
                    state.buttons_handler(action);

                    if (action != ButtonAction::Equals)
                        state.form_input_text();

                    return true;
                }
            }

            return false;
        });
}