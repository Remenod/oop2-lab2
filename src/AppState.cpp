#include "AppState.hpp"
#include "tinyexpr.h"
#include <memory>
#include <functional>
#include <iomanip>
#include <sstream>
#include <cmath>

static std::function<double()> compile_expr(const std::string &expr, int &err)
{
    te_expr *e_raw = te_compile(expr.c_str(), {}, 0, &err);

    if (err)
        return {};

    auto e = std::shared_ptr<te_expr>(e_raw, [](te_expr *ptr)
                                      { te_free(ptr); });

    return [e]()
    {
        return te_eval(e.get());
    };
}

static std::string double_to_string(double value, int precision = 10)
{
    if (std::isinf(value))
        return value > 0 ? "∞" : "-∞";
    if (std::isnan(value))
        return "Error";

    std::ostringstream oss;
    oss << std::setprecision(precision) << std::noshowpoint << value;
    std::string s = oss.str();

    if (s.find('.') != std::string::npos)
    {
        s.erase(s.find_last_not_of('0') + 1);
        if (s.back() == '.')
            s.pop_back();
    }

    return s;
}

// ── query helpers ─────────────────────────────────────────────────────────────

std::optional<AppState::TK> AppState::last_kind() const
{
    if (!num_buffer.empty())
        return TK::Number;
    if (tokens.empty())
        return std::nullopt;
    return tokens.back().kind;
}

bool AppState::is_value_start() const
{
    auto lk = last_kind();
    if (!lk)
        return true; // expression is empty
    return *lk == TK::BinaryOp || *lk == TK::UnaryMinus || *lk == TK::LParen;
}

bool AppState::can_append_op() const
{
    auto lk = last_kind();
    if (!lk)
        return false;
    return *lk == TK::Number || *lk == TK::RParen ||
           *lk == TK::Constant || *lk == TK::Percent;
}

int AppState::open_parens() const
{
    int n = 0;
    for (const auto &t : tokens)
    {
        if (t.kind == TK::LParen)
            ++n;
        else if (t.kind == TK::RParen)
            --n;
    }
    return n;
}

// ── mutation helpers ──────────────────────────────────────────────────────────

void AppState::finalize_number()
{
    if (!num_buffer.empty())
    {
        tokens.push_back({TK::Number, num_buffer, num_buffer});
        num_buffer.clear();
    }
}

void AppState::form_input_text()
{
    input_text.clear();
    func_text.clear();
    for (const auto &t : tokens)
    {
        input_text += t.display;
        func_text += t.func;
    }
    input_text += num_buffer;
    func_text += num_buffer;
}

void AppState::eval()
{
    std::string expr;
    expr.reserve(tokens.size() * 3 + num_buffer.size());
    for (const auto &t : tokens)
        expr += t.func;
    expr += num_buffer;

    int err = 0;
    auto fn = compile_expr(expr, err);
    if (!err)
        result_text = double_to_string(fn());
    else
        result_text = "Error";
}

void AppState::all_clear()
{
    tokens.clear();
    num_buffer.clear();
    result_text = "0";
}

// ── clear entry ───────────────────────────────────────────────────────────────

void AppState::clear_entry()
{
    if (!num_buffer.empty())
    {
        num_buffer.pop_back();
        return;
    }
    if (tokens.empty())
        return;

    // restore a finalized Number token back into the editable buffer
    if (tokens.back().kind == TK::Number)
    {
        num_buffer = tokens.back().display; // .display == .func for plain numbers
        tokens.pop_back();
        num_buffer.pop_back(); // remove the last digit
        return;
    }

    // A Function token is always followed immediately by an auto-inserted LParen.
    // Remove both together so we never leave a dangling "sin" with no "(".
    if (tokens.back().kind == TK::LParen &&
        tokens.size() >= 2 &&
        tokens[tokens.size() - 2].kind == TK::Function)
    {
        tokens.pop_back(); // remove '('
    }
    tokens.pop_back();
}

// ── main button handler ───────────────────────────────────────────────────────

void AppState::button_handler(ButtonAction act)
{
    bool state_changed = false;

    const bool val_start = is_value_start();
    const bool can_op = can_append_op();

    auto push = [&](TK kind, std::string_view disp, std::string_view fn)
    {
        tokens.push_back({kind, std::string(disp), std::string(fn)});
        state_changed = true;
    };

    auto implicit_mul = [&]()
    {
        if (can_op)
        {
            finalize_number();
            push(TK::BinaryOp, " × ", " * ");
        }
    };

    auto ensure_val_start = [&]() -> bool
    {
        if (val_start)
            return true;
        if (can_op)
        {
            implicit_mul();
            return true;
        }
        return false;
    };

    auto push_or_replace_binary = [&](std::string_view disp, std::string_view fn)
    {
        if (can_op)
        {
            finalize_number();
            push(TK::BinaryOp, disp, fn);
        }
        else if (!tokens.empty() && tokens.back().kind == TK::BinaryOp)
        {
            tokens.back() = {TK::BinaryOp, std::string(disp), std::string(fn)};
            state_changed = true;
        }
    };

    switch (act)
    {
    // ── digits ───────────────────────────────────────────────────────────────
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
    {
        char ch = '0' + (static_cast<int>(act) - static_cast<int>(ButtonAction::Digit0));

        // If no number is being built and we are not at a value-start position,
        // try to auto-insert multiplication (e.g. the user typed "5" then "3").
        if (num_buffer.empty() && !val_start)
        {
            if (!can_op)
                break;
            implicit_mul(); // inserts "×"; now we are effectively at val_start
        }

        if (num_buffer == "0")
        {
            if (ch == '0')
                break;          // "00" → still "0", nothing to do
            num_buffer.clear(); // drop the leading zero
        }

        num_buffer += ch;
        state_changed = true;
        break;
    }

    // ── decimal dot ──────────────────────────────────────────────────────────
    case ButtonAction::Dot:
        if (!num_buffer.empty())
        {
            if (num_buffer.find('.') == std::string::npos)
            {
                num_buffer += '.';
                state_changed = true;
            }
        }
        else if (val_start)
        {
            num_buffer = "0."; // auto-prefix: "." → "0."
            state_changed = true;
        }
        break;

    // ── parentheses ───────────────────────────────────────────────────────────
    case ButtonAction::LParen:
        if (ensure_val_start())
            push(TK::LParen, "(", "(");
        break;

    case ButtonAction::RParen:
        if (can_op && open_parens() > 0)
        {
            finalize_number();
            push(TK::RParen, ")", ")");
        }
        break;

    // ── binary operators ──────────────────────────────────────────────────────
    case ButtonAction::Add:
        push_or_replace_binary(" + ", " + ");
        break;
    case ButtonAction::Mul:
        push_or_replace_binary(" × ", " * ");
        break;
    case ButtonAction::Div:
        push_or_replace_binary(" ÷ ", " / ");
        break;
    case ButtonAction::Pow:
        push_or_replace_binary("^", "^");
        break;

    // ── subtract / unary-minus ────────────────────────────────────────────────
    case ButtonAction::Sub:
    case ButtonAction::UnaryMinus:
        if (val_start)
        {
            push(TK::UnaryMinus, "-", "-");
        }
        else if (can_op)
        {
            finalize_number();
            push(TK::BinaryOp, " - ", " - ");
        }
        else if (!tokens.empty() && tokens.back().kind == TK::BinaryOp)
        {
            tokens.back() = {TK::BinaryOp, " - ", " - "};
            state_changed = true;
        }
        break;

    // ── percent (postfix) ─────────────────────────────────────────────────────
    case ButtonAction::Percent:
        if (can_op)
        {
            finalize_number();
            push(TK::Percent, "%", "/100");
        }
        break;

    // ── prefix functions ──────────────────────────────────────────────────────
    case ButtonAction::Sin:
        if (ensure_val_start())
        {
            push(TK::Function, "sin", "sin");
            push(TK::LParen, "(", "(");
        }
        break;
    case ButtonAction::Cos:
        if (ensure_val_start())
        {
            push(TK::Function, "cos", "cos");
            push(TK::LParen, "(", "(");
        }
        break;
    case ButtonAction::Tan:
        if (ensure_val_start())
        {
            push(TK::Function, "tan", "tan");
            push(TK::LParen, "(", "(");
        }
        break;
    case ButtonAction::Ln:
        if (ensure_val_start())
        {
            push(TK::Function, "ln", "ln");
            push(TK::LParen, "(", "(");
        }
        break;
    case ButtonAction::Log:
        if (ensure_val_start())
        {
            push(TK::Function, "log", "log");
            push(TK::LParen, "(", "(");
        }
        break;
    case ButtonAction::Sqrt:
        if (ensure_val_start())
        {
            push(TK::Function, "√", "sqrt");
            push(TK::LParen, "(", "(");
        }
        break;

    // ── constants ─────────────────────────────────────────────────────────────
    case ButtonAction::Pi:
        if (ensure_val_start())
            push(TK::Constant, "π", "pi");
        break;
    case ButtonAction::E:
        if (ensure_val_start())
            push(TK::Constant, "e", "e");
        break;

    // ── clear operations ──────────────────────────────────────────────────────
    case ButtonAction::Clear:
        tokens.clear();
        num_buffer.clear();
        state_changed = true;
        break;

    case ButtonAction::AllClear:
        this->all_clear();
        state_changed = true;
        break;

    case ButtonAction::ClearEntry:
        this->clear_entry();
        state_changed = true;
        break;

    // ── evaluate ──────────────────────────────────────────────────────────────
    case ButtonAction::Equals:
        if (can_op && open_parens() == 0)
        {
            finalize_number();
            this->eval();
            state_changed = true;
        }
        break;
    }

    if (state_changed)
        this->form_input_text();
}