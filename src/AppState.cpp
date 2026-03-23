#include "AppState.hpp"
#include "tinyexpr.h"
#include <memory>
#include <functional>

using tfIsThat = std::array<std::string_view, AppState::COLS>;

typedef std::function<double(double)> mathfunc;

constexpr std::array<tfIsThat, AppState::ROWS> AppState::labels = {
    tfIsThat{" π ", " ! ", " ( ", " ) ", " % ", " C "},
    tfIsThat{" e ", "ln ", " 7 ", " 8 ", " 9 ", " / "},
    tfIsThat{"sin", "log", " 4 ", " 5 ", " 6 ", " * "},
    tfIsThat{"cos", " √ ", " 1 ", " 2 ", " 3 ", " - "},
    tfIsThat{"tan", " ^ ", " 0 ", " . ", " = ", " + "},
};

void AppState::eval(void)
{
    int func_err;
    auto func = get_func("", func_err);
}

void AppState::reset(void)
{
    this->input_text = "";
    this->result_text = "0";
}

void AppState::buttons_handler(size_t row, size_t col)
{
    switch (row)
    {
    case 0:
        switch (col)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        default:
            throw "ну бля";
        }
    case 1:
        switch (col)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        default:
            throw "ну бля";
        }
    case 2:
        switch (col)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        default:
            throw "ну бля";
        }
    case 3:
        switch (col)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        default:
            throw "ну бля";
        }
    case 4:
        switch (col)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        default:
            throw "ну бля";
        }
    default:
        throw "ну бля";
    }
}

static mathfunc get_func(const std::string &expr, int &err)
{
    auto te_x = std::make_shared<double>(0);

    te_variable vars[] = {{"x", te_x.get(), TE_VARIABLE, nullptr}};
    te_expr *e_raw = te_compile(expr.c_str(), vars, 1, &err);

    if (err)
        return {};

    auto e = std::shared_ptr<te_expr>(e_raw, [](te_expr *ptr)
                                      { te_free(ptr); });

    return [e, te_x](double x)
    {
        *te_x = x;
        return te_eval(e.get());
    };
}