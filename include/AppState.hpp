#pragma once

#include <string>
#include <array>

struct AppState
{
    static constexpr int ROWS = 5;
    static constexpr int COLS = 6;
    // static const char *labels[ROWS][COLS];
    static constexpr std::array<std::array<std::string_view, COLS>, ROWS> labels;

    std::string input_text;
    std::string result_text;

    void buttons_handler(size_t i, size_t j);
    void eval(void);
    void reset(void);
};
