#pragma once

#include <string>
#include <vector>
#include <optional>

enum class ButtonAction
{
    Equals,
    Clear,
    ClearEntry,
    AllClear,
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
    UnaryMinus,
    Percent,
    Pow,
    Sqrt,
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
    static inline const std::vector<std::vector<CalcButton>> buttons =
        {
            {
                {" π ", ButtonAction::Pi},
                {" ( ", ButtonAction::LParen},
                {" ) ", ButtonAction::RParen},
                {" % ", ButtonAction::Percent},
                {" C ", ButtonAction::Clear},
                {" CE", ButtonAction::ClearEntry},
                {" AC", ButtonAction::AllClear},
            },
            {
                {" e ", ButtonAction::E},
                {"ln ", ButtonAction::Ln},
                {" 7 ", ButtonAction::Digit7},
                {" 8 ", ButtonAction::Digit8},
                {" 9 ", ButtonAction::Digit9},
                {" / ", ButtonAction::Div},
            },
            {
                {"sin", ButtonAction::Sin},
                {"log", ButtonAction::Log},
                {" 4 ", ButtonAction::Digit4},
                {" 5 ", ButtonAction::Digit5},
                {" 6 ", ButtonAction::Digit6},
                {" * ", ButtonAction::Mul},
            },
            {
                {"cos", ButtonAction::Cos},
                {" √ ", ButtonAction::Sqrt},
                {" 1 ", ButtonAction::Digit1},
                {" 2 ", ButtonAction::Digit2},
                {" 3 ", ButtonAction::Digit3},
                {" - ", ButtonAction::Sub},
            },
            {
                {"tan", ButtonAction::Tan},
                {" ^ ", ButtonAction::Pow},
                {" 0 ", ButtonAction::Digit0},
                {" . ", ButtonAction::Dot},
                {" = ", ButtonAction::Equals},
                {" + ", ButtonAction::Add},
            },
    };

    std::string input_text;
    std::string result_text;
    std::string func_text;

    AppState() : result_text("0")
    {
        input_text.reserve(256);
        result_text.reserve(256);
        func_text.reserve(256);
    }

    void button_handler(ButtonAction btn);
    void eval(void);
    void form_input_text(void);
    void all_clear(void);
    void clear_entry(void);

private:
    struct TokenEntry
    {
        enum class Kind
        {
            Number,
            BinaryOp,
            UnaryMinus,
            Function,
            LParen,
            RParen,
            Constant,
            Percent
        };
        Kind kind;
        std::string display; // shown to the user
        std::string func;    // passed to tinyexpr
    };
    using TK = TokenEntry::Kind;

    std::vector<TokenEntry> tokens;
    std::string num_buffer; // digits being typed right now

    // helpers
    std::optional<TK> last_kind() const;
    bool is_value_start() const; // empty / after op / after '('
    bool can_append_op() const;  // after number / ')' / constant / '%'
    int open_parens() const;
    void finalize_number(); // flush num_buffer -> tokens
};