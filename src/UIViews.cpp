#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <vector>
#include <string>
#include "AppState.hpp"

using namespace ftxui;

Component DrawUI(AppState &state)
{

    auto container = Container::Vertical({});

    ButtonOption btn_option;
    btn_option.transform = [](const EntryState &s)
    {
        auto element = text(s.label) //
                       | center      //
                       | flex;

        if (s.active)
            element = element //
                      | bold;

        if (s.focused)
            element = element //
                      | inverted;

        return element //
               | border;
    };

    for (size_t i = 0; i < AppState::ROWS; ++i)
    {
        auto row_container = Container::Horizontal({});

        for (size_t j = 0; j < AppState::COLS; ++j)
        {
            row_container->Add(Button(
                AppState::labels[i][j].data(),
                [&state, i, j]
                {
                    state.buttons_handler(i, j);
                },
                btn_option));
        }

        container->Add(row_container);
    }

    return Renderer(
        container,
        [container, state]
        {
            auto display =
                vbox(
                    {
                        text(state.result_text)      //
                            | color(Color::GrayDark) //
                            | align_right,
                        text(state.input_text)       //
                            | size(HEIGHT, EQUAL, 1) //
                            | align_right            //
                            | bold,
                    })         //
                | borderDouble //
                | color(Color::BlueLight);

            Elements grid_elements;
            for (size_t i = 0; i < container->ChildCount(); ++i)
            {
                Elements row_elements;
                auto row = container->ChildAt(i);

                for (size_t j = 0; j < row->ChildCount(); ++j)
                {
                    row_elements.push_back(
                        row->ChildAt(j)->Render() //
                        | flex);
                }

                grid_elements.push_back(
                    hbox(std::move(row_elements)) //
                    | flex);
            }

            return vbox(
                       {
                           display,
                           vbox(std::move(grid_elements)) //
                               | flex                     //
                       })                                 //
                   | border;
        });
}
