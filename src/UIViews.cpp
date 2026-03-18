#include "UIViews.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

Component DrawUI(void)
{
    std::vector<std::vector<std::string>> labels = {
        {"7", "8", "9", "/", "C"},
        {"4", "5", "6", "*", "("},
        {"1", "2", "3", "-", ")"},
        {"0", ".", "=", "+", "^"},
        {"sin", "cos", "tan", "log", "ln"},
        {"pi", "e", "<-", "CE", "Ans"}};

    std::vector<std::vector<Component>> buttons_grid;
    std::vector<Component> rows;

    for (auto &row_labels : labels)
    {
        std::vector<Component> buttons;

        for (auto &label : row_labels)
        {
            buttons.push_back(Button(
                label,
                [label]
                {
                    //
                }));
        }

        buttons_grid.push_back(buttons);
        rows.push_back(Container::Horizontal(buttons));
    }

    auto container = Container::Vertical(rows);

    return Renderer(
        container,
        [buttons_grid]
        {
            std::vector<Element> row_elements;

            for (size_t i = 0; i < buttons_grid.size(); i++)
            {
                std::vector<Element> button_elements;

                for (size_t j = 0; j < buttons_grid[i].size(); j++)
                {
                    button_elements.push_back(
                        buttons_grid[i][j]->Render() | flex | size(WIDTH, GREATER_THAN, 10) | border | center);
                }

                row_elements.push_back(hbox(std::move(button_elements)));
            }

            return vbox(std::move(row_elements));
        });
}
