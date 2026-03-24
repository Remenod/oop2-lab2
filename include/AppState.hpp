#pragma once

#include <string>
#include <array>
#include <vector>

enum class ButtonAction
{
    Equals,
    Clear,
    Dot,
    LParen,
    RParen,
    Digit0,
    Digit1,
    Digit2,
    Digit3,
    Digit4,
    Digit5,
    Digit6,
    Digit7,
    Digit8,
    Digit9,
    Add,
    Sub,
    Mul,
    Div,
    Percent,
    Pow,
    Sqrt,
    Fact,
    Sin,
    Cos,
    Tan,
    Pi,
    E,
    Ln,
    Log,
};

struct CalcButton
{
    std::string_view label;
    ButtonAction action;
};

struct AppState
{
    static constexpr size_t ROWS = 5;
    static constexpr size_t COLS = 6;
    static constexpr std::array<std::array<CalcButton, COLS>, ROWS> buttons =
        {{
            {{
                {" π ", ButtonAction::Pi},
                {" ! ", ButtonAction::Fact},
                {" ( ", ButtonAction::LParen},
                {" ) ", ButtonAction::RParen},
                {" % ", ButtonAction::Percent},
                {"CE ", ButtonAction::Clear} //
            }},
            {{
                {" e ", ButtonAction::E},
                {"ln ", ButtonAction::Ln},
                {" 7 ", ButtonAction::Digit7},
                {" 8 ", ButtonAction::Digit8},
                {" 9 ", ButtonAction::Digit9},
                {" / ", ButtonAction::Div} //
            }},
            {{
                {"sin", ButtonAction::Sin},
                {"log", ButtonAction::Log},
                {" 4 ", ButtonAction::Digit4},
                {" 5 ", ButtonAction::Digit5},
                {" 6 ", ButtonAction::Digit6},
                {" * ", ButtonAction::Mul} //
            }},
            {{
                {"cos", ButtonAction::Cos},
                {" √ ", ButtonAction::Sqrt},
                {" 1 ", ButtonAction::Digit1},
                {" 2 ", ButtonAction::Digit2},
                {" 3 ", ButtonAction::Digit3},
                {" - ", ButtonAction::Sub} //
            }},
            {{
                {"tan", ButtonAction::Tan},
                {" ^ ", ButtonAction::Pow},
                {" 0 ", ButtonAction::Digit0},
                {" . ", ButtonAction::Dot},
                {" = ", ButtonAction::Equals},
                {" + ", ButtonAction::Add} //
            }} //
        }};

    std::string input_text;
    std::string result_text;

    std::vector<ButtonAction> actionSequence;

    AppState() : input_text(""), result_text("0")
    {
        input_text.reserve(64);
        result_text.reserve(64);
    }

    void buttons_handler(ButtonAction btn);
    void eval(void);
    void reset(void);
};
