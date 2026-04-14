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

static std::string double_to_string(double value, int precision = 6)
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
    return *lk == TK::Number || *lk == TK::RParen || *lk == TK::Constant || *lk == TK::Percent;
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

    // A Function token is always followed immediately by an auto-inserted LParen.
    // Remove both together so we never leave a dangling "sin" with no "(".
    if (tokens.back().kind == TK::LParen && tokens.size() >= 2 && tokens[tokens.size() - 2].kind == TK::Function)
    {
        tokens.pop_back(); // remove '('
    }
    tokens.pop_back();
}

// ── main button handler ───────────────────────────────────────────────────────

void AppState::button_handler(ButtonAction act)
{
    bool state_changed = false;

    // Snapshot context before any mutation.
    const bool val_start = is_value_start();
    const bool can_op = can_append_op();

    // Shorthand: push a token and mark the state as changed.
    auto push = [&](TK kind, std::string_view disp, std::string_view fn)
    {
        tokens.push_back({kind, std::string(disp), std::string(fn)});
        state_changed = true;
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
        // A digit may be typed only at the start of a value or while
        // we are already building a number.
        if (num_buffer.empty() && !val_start)
            break;

        // Prevent leading zeros: "0" + any digit = illegal (but "0." is fine,
        // handled by the Dot case).
        if (num_buffer == "0")
            break;

        char ch = '0' + (static_cast<int>(act) - static_cast<int>(ButtonAction::Digit0));
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
        if (val_start)
        {
            finalize_number(); // safety; buffer is empty when val_start is true
            push(TK::LParen, "(", "(");
        }
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
        if (can_op)
        {
            finalize_number();
            push(TK::BinaryOp, " + ", " + ");
        }
        break;
    case ButtonAction::Mul:
        if (can_op)
        {
            finalize_number();
            push(TK::BinaryOp, " * ", " * ");
        }
        break;
    case ButtonAction::Div:
        if (can_op)
        {
            finalize_number();
            push(TK::BinaryOp, " / ", " / ");
        }
        break;
    case ButtonAction::Pow:
        if (can_op)
        {
            finalize_number();
            push(TK::BinaryOp, "^", "^");
        }
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
        if (val_start)
        {
            push(TK::Function, "sin", "sin");
            push(TK::LParen, "(", "(");
        }
        break;
    case ButtonAction::Cos:
        if (val_start)
        {
            push(TK::Function, "cos", "cos");
            push(TK::LParen, "(", "(");
        }
        break;
    case ButtonAction::Tan:
        if (val_start)
        {
            push(TK::Function, "tan", "tan");
            push(TK::LParen, "(", "(");
        }
        break;
    case ButtonAction::Ln:
        if (val_start)
        {
            push(TK::Function, "ln", "ln");
            push(TK::LParen, "(", "(");
        }
        break;
    case ButtonAction::Log:
        if (val_start)
        {
            push(TK::Function, "log", "log");
            push(TK::LParen, "(", "(");
        }
        break;
    case ButtonAction::Sqrt:
        if (val_start)
        {
            push(TK::Function, "√", "sqrt");
            push(TK::LParen, "(", "(");
        }
        break;

    // ── constants ─────────────────────────────────────────────────────────────
    case ButtonAction::Pi:
        if (val_start)
        {
            push(TK::Constant, "π", "pi");
        }
        break;
    case ButtonAction::E:
        if (val_start)
        {
            push(TK::Constant, "e", "e");
        }
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