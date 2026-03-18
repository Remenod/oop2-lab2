#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <vector>
#include <string>

using namespace ftxui;

Component DrawUI()
{
    static std::string input_text = "0";
    static std::string result_text = "";

    std::vector<std::vector<std::string>> labels = {
        {"pi", "!", "(", ")", "%", "C"},
        {"e", "ln", "7", "8", "9", "/"},
        {"sin", "log", "4", "5", "6", "*"},
        {"cos", "sqrt", "1", "2", "3", "-"},
        {"tan", "^", "0", ".", "=", "+"},
    };

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

    for (const auto &row_labels : labels)
    {
        auto row_container = Container::Horizontal({});

        for (const auto &label : row_labels)
            row_container->Add(Button(
                label,
                [label]
                {
                    //
                },
                btn_option));

        container->Add(row_container);
    }

    return Renderer(
        container,
        [container]
        {
            auto display = vbox(
                               {
                                   text(result_text)            //
                                       | color(Color::GrayDark) //
                                       | align_right,
                                   text(input_text)             //
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
