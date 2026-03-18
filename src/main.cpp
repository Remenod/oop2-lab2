#include <iostream>
#include <vector>
#include <functional>
#include <fstream>
#include <string>
#include <memory>
#include "lib/tinyexpr.h"

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

int main()
{
    int func_err;
    auto func = get_func("", func_err);

    return 0;
}
