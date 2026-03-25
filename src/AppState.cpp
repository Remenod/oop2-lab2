#include "AppState.hpp"
#include "tinyexpr.h"
#include <memory>
#include <functional>
#include <algorithm>
#include <sstream>
#include <string>
#include <iomanip>
#include <unordered_map>
#include <string_view>

static std::function<double(void)> get_func(const std::string &expr, int &err)
{
    auto te_x = std::make_shared<double>(0);

    te_expr *e_raw = te_compile(expr.c_str(), {}, 1, &err);

    if (err)
        return {};

    auto e = std::shared_ptr<te_expr>(e_raw, [](te_expr *ptr)
                                      { te_free(ptr); });

    return [e]()
    {
        return te_eval(e.get());
    };
}

static std::string doubleToString(double value, int precision = 6)
{
    std::ostringstream oss;
    oss << std::setprecision(precision) << std::noshowpoint << value;
    std::string s = oss.str();

    if (s.find('.') != std::string::npos)
    {
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.')
            s.pop_back();
    }

    return s;
}

void AppState::eval(void)
{
    int func_err;
    auto func = get_func(this->func_text, func_err);
    if (!func_err)
        this->result_text = doubleToString(func());
    else
    {
        // idk
    }
}

void AppState::form_input_text(void)
{
    this->input_text.clear();
    this->func_text.clear();

    this->input_text.reserve(this->actionSequence.size() * 2);
    this->func_text.reserve(this->actionSequence.size() * 3);

    auto last_ce = std::find(
        this->actionSequence.rbegin(),
        this->actionSequence.rend(),
        ButtonAction::ClearEntry);

    auto start_iter = (last_ce != this->actionSequence.rend())
                          ? std::next(last_ce.base())
                          : this->actionSequence.begin();

    struct TokenInfo
    {
        std::string_view input;
        std::string_view func;
    };

    static const std::unordered_map<ButtonAction, TokenInfo> token_map = {
        {ButtonAction::Digit0, {"0", "0"}},
        {ButtonAction::Digit1, {"1", "1"}},
        {ButtonAction::Digit2, {"2", "2"}},
        {ButtonAction::Digit3, {"3", "3"}},
        {ButtonAction::Digit4, {"4", "4"}},
        {ButtonAction::Digit5, {"5", "5"}},
        {ButtonAction::Digit6, {"6", "6"}},
        {ButtonAction::Digit7, {"7", "7"}},
        {ButtonAction::Digit8, {"8", "8"}},
        {ButtonAction::Digit9, {"9", "9"}},

        {ButtonAction::Dot, {".", "."}},
        {ButtonAction::Add, {" + ", " + "}},
        {ButtonAction::Sub, {" - ", " - "}},
        {ButtonAction::Mul, {" * ", " * "}},
        {ButtonAction::Div, {" / ", " / "}},
        {ButtonAction::Percent, {"%", "/100"}},
        {ButtonAction::UnaryMinus, {"-", "-"}},
        {ButtonAction::LParen, {"(", "("}},
        {ButtonAction::RParen, {")", ")"}},

        {ButtonAction::Cos, {"cos", "cos"}},
        {ButtonAction::Sin, {"sin", "sin"}},
        {ButtonAction::Tan, {"tan", "tan"}},
        {ButtonAction::Ln, {"ln", "ln"}},
        {ButtonAction::Log, {"log", "log"}},
        {ButtonAction::Sqrt, {"√", "sqrt"}},
        {ButtonAction::Pow, {"^", "^"}},
        {ButtonAction::Pi, {"π", "pi"}},
        {ButtonAction::E, {"e", "e"}}};

    for (auto iter = start_iter; iter != this->actionSequence.end(); ++iter)
    {
        if (*iter == ButtonAction::Clear)
        {
            this->input_text.clear();
            this->func_text.clear();
            continue;
        }

        auto match = token_map.find(*iter);
        if (match != token_map.end())
        {
            this->input_text += match->second.input;
            this->func_text += match->second.func;
        }
    }
}
void AppState::all_clear(void)
{
    this->result_text = "0";
    this->actionSequence.clear();
}
void AppState::clear_entry(void)
{
    if (!this->actionSequence.empty() && this->actionSequence.back() != ButtonAction::Clear)
        this->actionSequence.pop_back();
}

void AppState::buttons_handler(ButtonAction act)
{
    auto is_digit = [](ButtonAction a)
    {
        return a >= ButtonAction::Digit0 && a <= ButtonAction::Digit9;
    };

    auto is_binary_operator = [](ButtonAction a)
    {
        return a == ButtonAction::Add || a == ButtonAction::Sub ||
               a == ButtonAction::Mul || a == ButtonAction::Div || a == ButtonAction::Pow;
    };

    auto is_postfix_operator = [](ButtonAction a)
    {
        return a == ButtonAction::Percent;
    };

    auto is_constant = [](ButtonAction a)
    {
        return a == ButtonAction::Pi || a == ButtonAction::E;
    };

    auto is_prefix_function = [](ButtonAction a)
    {
        return a == ButtonAction::Sin || a == ButtonAction::Cos ||
               a == ButtonAction::Tan || a == ButtonAction::Sqrt ||
               a == ButtonAction::Ln || a == ButtonAction::Log;
    };

    ButtonAction last = actionSequence.empty() ? ButtonAction::Clear : actionSequence.back();

    switch (act)
    {
    case ButtonAction::Digit0:
    case ButtonAction::Digit1:
    case ButtonAction::Digit2:
    case ButtonAction::Digit3:
    case ButtonAction::Digit4:
    case ButtonAction::Digit5:
    case ButtonAction::Digit6:
    case ButtonAction::Digit7:
    case ButtonAction::Digit8:
    case ButtonAction::Digit9:
        if (last == ButtonAction::RParen || is_constant(last) || is_postfix_operator(last))
            break;
        actionSequence.emplace_back(act);
        break;

    case ButtonAction::Dot:
        if (is_digit(last))
        {
            bool has_dot = false;
            for (auto it = actionSequence.rbegin(); it != actionSequence.rend(); ++it)
            {
                if (*it == ButtonAction::Dot)
                {
                    has_dot = true;
                    break;
                }

                if (!is_digit(*it))
                    break;
            }

            if (!has_dot)
                actionSequence.emplace_back(act);
        }
        break;

    case ButtonAction::LParen:
        if (actionSequence.empty() || last == ButtonAction::Clear || last == ButtonAction::UnaryMinus || is_binary_operator(last) ||
            last == ButtonAction::LParen || is_prefix_function(last))
            actionSequence.emplace_back(act);
        break;

    case ButtonAction::RParen:
        if (is_digit(last) || last == ButtonAction::RParen || is_constant(last) || is_postfix_operator(last))
        {
            int open_parens = 0;
            for (auto a : actionSequence)
            {
                if (a == ButtonAction::LParen)
                    open_parens++;
                if (a == ButtonAction::RParen)
                    open_parens--;
            }

            if (open_parens > 0)
                actionSequence.emplace_back(act);
        }
        break;

    case ButtonAction::UnaryMinus:
    case ButtonAction::Sub:
        if (actionSequence.empty() || last == ButtonAction::Clear || last == ButtonAction::LParen)
            actionSequence.emplace_back(ButtonAction::UnaryMinus);
        else if (is_digit(last) || last == ButtonAction::RParen || is_constant(last) || is_postfix_operator(last))
            actionSequence.emplace_back(ButtonAction::Sub);
        break;

    case ButtonAction::Add:
    case ButtonAction::Mul:
    case ButtonAction::Div:
    case ButtonAction::Pow:
        if (is_digit(last) || last == ButtonAction::RParen || is_constant(last) || is_postfix_operator(last))
            actionSequence.emplace_back(act);
        break;

    case ButtonAction::Sin:
    case ButtonAction::Cos:
    case ButtonAction::Tan:
    case ButtonAction::Sqrt:
    case ButtonAction::Ln:
    case ButtonAction::Log:
        if (actionSequence.empty() || last == ButtonAction::Clear || last == ButtonAction::UnaryMinus || is_binary_operator(last) || last == ButtonAction::LParen)
        {
            actionSequence.emplace_back(act);
            actionSequence.emplace_back(ButtonAction::LParen);
        }
        break;

    case ButtonAction::Percent:
        if (is_digit(last) || last == ButtonAction::RParen || is_constant(last) || is_postfix_operator(last))
            actionSequence.emplace_back(act);
        break;

    case ButtonAction::Pi:
    case ButtonAction::E:
        if (actionSequence.empty() || last == ButtonAction::Clear || last == ButtonAction::UnaryMinus || is_binary_operator(last) || last == ButtonAction::LParen)
            actionSequence.emplace_back(act);
        break;

    case ButtonAction::Clear:
        actionSequence.emplace_back(act);
        break;

    case ButtonAction::AllClear:
        this->all_clear();
        break;
    case ButtonAction::ClearEntry:
        this->clear_entry();
        break;

    case ButtonAction::Equals:
    {
        bool ends_with_invalid_token =
            is_binary_operator(last) ||
            last == ButtonAction::LParen ||
            last == ButtonAction::Dot ||
            last == ButtonAction::UnaryMinus ||
            is_prefix_function(last);

        int open_parens_count = 0;
        for (auto act : this->actionSequence)
        {
            if (act == ButtonAction::LParen)
                open_parens_count++;
            else if (act == ButtonAction::RParen)
                open_parens_count--;
        }
        bool parens_balanced = (open_parens_count == 0);

        if (!ends_with_invalid_token && parens_balanced)
            this->eval();
        break;
    }
    }
    this->form_input_text();
}
