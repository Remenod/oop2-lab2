#include "AppState.hpp"
#include "tinyexpr.h"
#include <memory>
#include <functional>
#include <algorithm>
#include <sstream>
#include <string>
#include <iomanip>

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
    this->input_text = "";
    this->func_text = "";

    auto last_ce = std::find(this->actionSequence.rbegin(), this->actionSequence.rend(), ButtonAction::ClearEntry);

    auto start_iter = (last_ce != this->actionSequence.rend()) ? std::next(last_ce.base()) : this->actionSequence.begin();

    for (auto iter = start_iter; iter != this->actionSequence.end(); ++iter)
    {
        switch (*iter)
        {
        case ButtonAction::Digit0:
            this->input_text += "0";
            this->func_text += "0";
            break;
        case ButtonAction::Digit1:
            this->input_text += "1";
            this->func_text += "1";
            break;
        case ButtonAction::Digit2:
            this->input_text += "2";
            this->func_text += "2";
            break;
        case ButtonAction::Digit3:
            this->input_text += "3";
            this->func_text += "3";
            break;
        case ButtonAction::Digit4:
            this->input_text += "4";
            this->func_text += "4";
            break;
        case ButtonAction::Digit5:
            this->input_text += "5";
            this->func_text += "5";
            break;
        case ButtonAction::Digit6:
            this->input_text += "6";
            this->func_text += "6";
            break;
        case ButtonAction::Digit7:
            this->input_text += "7";
            this->func_text += "7";
            break;
        case ButtonAction::Digit8:
            this->input_text += "8";
            this->func_text += "8";
            break;
        case ButtonAction::Digit9:
            this->input_text += "9";
            this->func_text += "9";
            break;
        case ButtonAction::Dot:
            this->input_text += ".";
            this->func_text += ".";
            break;
        case ButtonAction::Add:
            this->input_text += " + ";
            this->func_text += " + ";
            break;
        case ButtonAction::Sub:
            this->input_text += " - ";
            this->func_text += " - ";
            break;
        case ButtonAction::Mul:
            this->input_text += " * ";
            this->func_text += " * ";
            break;
        case ButtonAction::Div:
            this->input_text += " / ";
            this->func_text += " / ";
            break;
        case ButtonAction::Percent:
            this->input_text += "%";
            this->func_text += "/100";
            break;
        case ButtonAction::UnaryMinus:
            this->input_text += " -";
            this->func_text += " -";
            break;
        case ButtonAction::LParen:
            this->input_text += "(";
            this->func_text += "(";
            break;
        case ButtonAction::RParen:
            this->input_text += ")";
            this->func_text += ")";
            break;
        case ButtonAction::Cos:
            this->input_text += "cos";
            this->func_text += "cos";
            break;
        case ButtonAction::Sin:
            this->input_text += "sin";
            this->func_text += "sin";
            break;
        case ButtonAction::Tan:
            this->input_text += "tan";
            this->func_text += "tan";
            break;
        case ButtonAction::Ln:
            this->input_text += "ln";
            this->func_text += "ln";
            break;
        case ButtonAction::Log:
            this->input_text += "log";
            this->func_text += "log";
            break;
        case ButtonAction::Sqrt:
            this->input_text += "√";
            this->func_text += "sqrt";
            break;
        case ButtonAction::Pow:
            this->input_text += "^";
            this->func_text += "^";
            break;
        case ButtonAction::Pi:
            this->input_text += "π";
            this->func_text += "pi";
            break;
        case ButtonAction::E:
            this->input_text += "e";
            this->func_text += "e";
            break;

        case ButtonAction::ClearEntry:
            this->input_text += "";
            this->func_text = "";
            break;
        case ButtonAction::AllClear:
        case ButtonAction::Clear:
        case ButtonAction::Equals:
            break;
        }
    }
}
void AppState::all_clear(void)
{
    this->actionSequence.clear();
}
void AppState::clear_entry(void)
{
    if (!this->actionSequence.empty())
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
        if (actionSequence.empty() || is_binary_operator(last) || last == ButtonAction::LParen)
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
        if (actionSequence.empty() || last == ButtonAction::LParen)
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
        if (actionSequence.empty() || is_binary_operator(last) || last == ButtonAction::LParen)
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
        if (actionSequence.empty() || is_binary_operator(last) || last == ButtonAction::LParen)
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
        if (!(is_binary_operator(last) || last == ButtonAction::LParen || last == ButtonAction::Dot))
            this->eval();
        break;
    }
    this->form_input_text();
}
