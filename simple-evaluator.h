#pragma once
#include <string>

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
#include <optional>
#endif

namespace seval {
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    std::optional<double> evalExpr(std::string expression);
#else
    bool checkSyntax(std::string expression);
    double evalExpr(std::string expression);
#endif
}
