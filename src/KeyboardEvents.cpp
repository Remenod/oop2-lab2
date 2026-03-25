#include "KeyboardEvents.hpp"
#include <ftxui/component/screen_interactive.hpp>

using namespace ftxui;

ftxui::ComponentDecorator GetEventCatcher(AppState &state)
{
    return CatchEvent(
        [&](Event event)
        {
            if (event == Event::Character('p'))
            {
                state.buttons_handler(ButtonAction::Pi);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('e'))
            {
                state.buttons_handler(ButtonAction::E);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('('))
            {
                state.buttons_handler(ButtonAction::LParen);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character(')'))
            {
                state.buttons_handler(ButtonAction::RParen);
                state.form_input_text();
                return true;
            }
            if (event == Event::Backspace)
            {
                state.buttons_handler(ButtonAction::ClearEntry);
                state.form_input_text();
                return true;
            }

            if (event == Event::Character('l'))
            {
                state.buttons_handler(ButtonAction::Ln);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('g'))
            {
                state.buttons_handler(ButtonAction::Log);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('q'))
            {
                state.buttons_handler(ButtonAction::Sqrt);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('s'))
            {
                state.buttons_handler(ButtonAction::Sin);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('c'))
            {
                state.buttons_handler(ButtonAction::Cos);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('t'))
            {
                state.buttons_handler(ButtonAction::Tan);
                state.form_input_text();
                return true;
            }

            if (event == Event::Character('0'))
            {
                state.buttons_handler(ButtonAction::Digit0);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('1'))
            {
                state.buttons_handler(ButtonAction::Digit1);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('2'))
            {
                state.buttons_handler(ButtonAction::Digit2);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('3'))
            {
                state.buttons_handler(ButtonAction::Digit3);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('4'))
            {
                state.buttons_handler(ButtonAction::Digit4);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('5'))
            {
                state.buttons_handler(ButtonAction::Digit5);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('6'))
            {
                state.buttons_handler(ButtonAction::Digit6);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('7'))
            {
                state.buttons_handler(ButtonAction::Digit7);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('8'))
            {
                state.buttons_handler(ButtonAction::Digit8);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('9'))
            {
                state.buttons_handler(ButtonAction::Digit9);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('.'))
            {
                state.buttons_handler(ButtonAction::Dot);
                state.form_input_text();
                return true;
            }

            if (event == Event::Character('+'))
            {
                state.buttons_handler(ButtonAction::Add);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('-'))
            {
                state.buttons_handler(ButtonAction::Sub);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('*'))
            {
                state.buttons_handler(ButtonAction::Mul);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('/'))
            {
                state.buttons_handler(ButtonAction::Div);
                state.form_input_text();
                return true;
            }
            if (event == Event::Character('^'))
            {
                state.buttons_handler(ButtonAction::Pow);
                return true;
            }
            if (event == Event::Character('=') || event == Event::Return)
            {
                state.buttons_handler(ButtonAction::Equals);
                return true;
            }
            if (event == Event::Character('%'))
            {
                state.buttons_handler(ButtonAction::Percent);
                return true;
            }

            return false;
        });
}