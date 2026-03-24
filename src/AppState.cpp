#include "AppState.hpp"
#include "tinyexpr.h"
#include <memory>
#include <functional>

typedef std::function<double(double)> mathfunc;

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
        return a == ButtonAction::Percent || a == ButtonAction::Fact;
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

    case ButtonAction::Sub:
        if (actionSequence.empty() || last == ButtonAction::LParen)
            actionSequence.emplace_back(act);
        else if (is_digit(last) || last == ButtonAction::RParen || is_constant(last) || is_postfix_operator(last))
            actionSequence.emplace_back(act);
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

    case ButtonAction::Fact:
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
        actionSequence.clear();
        break;

    case ButtonAction::Equals:
        if (!(is_binary_operator(last) || last == ButtonAction::LParen || last == ButtonAction::Dot))
            this->eval();
        break;
    }
}
