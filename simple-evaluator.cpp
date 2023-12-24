#include "simple-evaluator.h"
#include <vector>
#include <map>
#include <functional>
#include <stack>
#include <utility>
#include <cmath>
#include <algorithm>

typedef std::stack<std::string>& stack_t;
typedef std::vector<std::string>& vec_t;

bool isTopOperatorEqualTo(const std::stack<std::string>& operators, const std::initializer_list<std::string>& operatorList) {
    return !operators.empty() && std::find(operatorList.begin(), operatorList.end(), operators.top()) != operatorList.end();
}

bool compareCharToList(char ch, const std::initializer_list<char>& list) {
    return std::find(list.begin(), list.end(), ch) != list.end();
}

bool isSubstring(const std::string& expr, int i, const std::string& pattern) {
    if (pattern.size() == 5 && expr.size() - (i + 1) < 5 
     || pattern.size() == 4 && expr.size() - (i + 1) < 4 
     || pattern.size() == 3 && expr.size() - (i + 1) < 3) {
        return false;
    }

    for (int j = 0; j < pattern.size(); j++) {
        if (expr[i + j] != pattern[j]) {
            return false;
        }
    }
    return true;
}

bool isSquareCubicRoot(const std::string& expr, int i) {
    return (isSubstring(expr, i, "sq") || isSubstring(expr, i, "cb"));
}

bool isLogarithmicFunction(const std::string& expr, int i) {
    return (isSubstring(expr, i, "log") || isSubstring(expr, i, "ln") || isSubstring(expr, i, "exp"));
}

bool isTrigonometricFunction(const std::string& expr, int i) {
    return (isSubstring(expr, i, "sin") || isSubstring(expr, i, "cos") || isSubstring(expr, i, "tan"));
}

bool isHyperbolicFunction(const std::string& expr, int i) {
    return (isSubstring(expr, i, "sinh") || isSubstring(expr, i, "cosh") || isSubstring(expr, i, "tanh")
        || isSubstring(expr, i, "coth") || isSubstring(expr, i, "sech") || isSubstring(expr, i, "csch"));
}

bool isInverseHyperbolicFunction(const std::string& expr, int i) {
    return (isSubstring(expr, i, "asinh") || isSubstring(expr, i, "acosh") || isSubstring(expr, i, "atanh")
        || isSubstring(expr, i, "acoth") || isSubstring(expr, i, "asech") || isSubstring(expr, i, "acsch"));
}

bool isFunction(const std::string& expr, int i) {
    return (isLogarithmicFunction(expr, i) || isTrigonometricFunction(expr, i) || isHyperbolicFunction(expr, i) || isInverseHyperbolicFunction(expr, i));
}

bool isLn(const std::string& expr, int i) {
    return isSubstring(expr, i, "ln");
}

bool isLog2(const std::string& expr, int i) {
    return isSubstring(expr, i, "log2");
}

bool isTopHighPrecedenceOperator(const stack_t operators) {
    return isTopOperatorEqualTo(operators, 
        { "sqrt", "cbrt", 
          "sin", "cos", "tan", "cot", "sec", "csc", 
          "log", "log2", "ln", "exp",
          "sinh", "cosh", "tanh", "coth", "sech", "csch",
          "asinh", "acosh", "atanh", "acoth", "asech", "acsch",
          "^"});
}

bool isDigit(const std::string& expr, int i) {
    return (std::isdigit(expr[i]) || expr[i] == 'q' && !isFunction(expr, i + 1) && !isSquareCubicRoot(expr, i + 1) || expr[i] == '.');
}

bool isBinaryOp(const std::string& expr, int i) {
    return compareCharToList(expr[i], { '+', '-', '*', '/', '^', '%' });
}

bool isBinaryOpNoMinus(const std::string& expr, int i) {
    return compareCharToList(expr[i], { '+', '*', '/', '^', '%' });
}

bool isMissingMultiplication(const std::string& expr, int i) {
    if (i > 0) {
        if (expr[i] == '2' && expr[i - 1] == 'g') {
            return false;
        }
    }

    return (std::isdigit(expr[i]) && expr[i + 1] == '(' || expr[i] == ')' && std::isdigit(expr[i + 1]) || isSubstring(expr, i, ")("));
}

bool isPointlessExpr(const std::string& expr) {
    size_t operatorCounter = 0;
    for (int i = 0; i < expr.length(); i++) {
        if (i != expr.length() - 1) {
            if (isSubstring(expr, i, "(-")) {
                if (i == 0) {
                    i++;
                }
                i++;
                continue;
            }
        }
        if (isBinaryOp(expr, i) || isSquareCubicRoot(expr, i) || isFunction(expr, i)) {
            operatorCounter++;

            return false;
        }
    }
    return true;
}

void lexMultiplicationDivisionModulus(const std::string& expr, int i, stack_t operands, stack_t operators) {
    if (isTopHighPrecedenceOperator(operators) || isTopOperatorEqualTo(operators, { "*", "/", "%" })) {
        operands.push(operators.top());
        operators.pop();
    }
    operators.push(std::string(1, expr[i]));
}

void lexNumbers(std::string& expr, int& i, stack_t operands, stack_t operators) {
    std::string multiDigitNum{};
    if (std::isdigit(expr[i + 1]) || expr[i] == 'q' && !isFunction(expr, i + 1) && !isSquareCubicRoot(expr, i + 1) || expr[i + 1] == '.') {
        while (isDigit(expr, i)) {
            if (expr[i] == 'q') {
                expr.replace(i, 1, 1, '-');
            }
            multiDigitNum += std::string(1, expr[i]);
            i++;
        }
        operands.push(multiDigitNum);
        i--;
    }
    else {
        operands.push(std::string(1, expr[i]));
    }
}

void lexAdditionSubtraction(const std::string& expr, int i, stack_t operands, stack_t operators) {
    if (isTopOperatorEqualTo(operators, { "*", "/", "%"}) || isTopHighPrecedenceOperator(operators)) {
        while (operators.size() != 0) {
            operands.push(operators.top());
            operators.pop();
        }
    }
    else if (isTopOperatorEqualTo(operators, { "+", "-" })) {
        operands.push(operators.top());
        operators.pop();
    }
    operators.push(std::string(1, expr[i]));
}

void pushFunctionToOps(const std::string& expr, int i, stack_t& operators, bool isNegative = false) {
    if (isNegative) {
        if (isLn(expr, i)) {
            operators.push(expr.substr(i, 3));
        }
        else if (isLog2(expr, i) || isHyperbolicFunction(expr, i)) {
            operators.push(expr.substr(i, 5));
        }
        else if (isInverseHyperbolicFunction(expr, i)) {
            operators.push(expr.substr(i, 6));
        }
        else {
            operators.push(expr.substr(i, 4));
        }
    }
    else {
        if (isLn(expr, i)) {
            operators.push(expr.substr(i, 2));
        }
        else if (isLog2(expr, i) || isHyperbolicFunction(expr, i)) {
            operators.push(expr.substr(i, 4));
        }
        else if (isInverseHyperbolicFunction(expr, i)) {
            operators.push(expr.substr(i, 5));
        }
        else {
            operators.push(expr.substr(i, 3));
        }
    }
}

void lexFunction(std::string& expr, int& i, stack_t& operands, stack_t& operators) {
    if (isTopHighPrecedenceOperator(operators)) {
        operands.push(operators.top());
        operators.pop();

        pushFunctionToOps(expr, i, operators);
    }
    else if (expr[i] == 'q') {
        expr.replace(i, 1, 1, '-');

        pushFunctionToOps(expr, i, operators, true);
    }
    else if (i > 0) {
        if (expr[i - 1] != '-') {
            pushFunctionToOps(expr, i, operators);
        }
    }
    else if (i == 0) {
        pushFunctionToOps(expr, i, operators);
    }

    if (isLn(expr, i)) {
        i += 1;
    }
    else if (isLog2(expr, i) || isHyperbolicFunction(expr, i)) {
        i += 3;
    }
    else if (isInverseHyperbolicFunction(expr, i)) {
        i += 4;
    }
    else {
        i += 2;
    }
}

void lexSqrtCbrt(std::string& expr, int& i, stack_t& operands, stack_t& operators) {
    if (isTopHighPrecedenceOperator(operators)) {
        operands.push(operators.top());
        operators.pop();
        operators.push(expr.substr(i, 4));
    }
    else if (expr[i] == 'q') {
        expr.replace(i, 1, 1, '-');
        operators.push(expr.substr(i, 5));
    }
    else if (i > 0) {
        if (expr[i - 1] != '-') {
            operators.push(expr.substr(i, 4));
        }
    }
    else if (i == 0) {
        operators.push(expr.substr(i, 4));
    }
    i += 3;
}

void lexPow(std::string& expr, int i, stack_t operands, stack_t operators) {
    if (isTopHighPrecedenceOperator(operators)) {
        operands.push(operators.top());
        operators.pop();
    }
    operators.push(std::string(1, expr[i]));
}

void lexBrackets(char bracketType, std::string& expr, int i, stack_t operands, stack_t operators) {
    if (bracketType == '(') {
        if (expr[i + 1] == '-') {
            expr.replace(i + 1, 1, 1, 'q');
        }
        operators.push(std::string(1, expr[i]));
    }
    else {
        while (!operators.empty() && operators.top() != "(") {
            operands.push(operators.top());
            operators.pop();
        }
        if (!operators.empty()) {
            operators.pop();
        }
    }
}

void addMissingMultiplication(std::string& expr) {
    for (int i = 0; i < expr.size(); i++) {
        if (isMissingMultiplication(expr, i)) {
            expr.insert(expr.begin() + (i + 1), '*');
        }
    }
}

void solveMultipleMinuses(std::string& expr) {
    size_t minusCounter = 0, minusIndex = 0;
    for (int i = 0; i < expr.length(); i++) {
        if (isSubstring(expr, i, "(-")) {
            minusCounter++;

            expr.erase(expr.begin() + (i + 1));
            minusIndex = i + 1;
        }
        else if (isSubstring(expr, i, "-(") && i == 0) {
            minusCounter++;

            expr.erase(expr.begin() + i);
            minusIndex = i;
            i--;
        }
        else if (isSubstring(expr, i, "-(") && minusCounter != 0) {
            minusCounter++;

            expr.erase(expr.begin() + i);
            minusIndex = i;
        }
    }

    if (minusCounter & 1) {
        expr.insert(expr.begin() + minusIndex, '-');
    }
}

void eraseWhiteSpaces(std::string& expr) {
    expr.erase(std::remove_if(expr.begin(), expr.end(), [](char ch) { return ch == ' '; }), expr.end());
}

void eraseBrackets(std::string& expr) {
    expr.erase(std::remove_if(expr.begin(), expr.end(), [](char ch) { return ch == '(' || ch == ')'; }), expr.end());
}

void shuntingYardAlgorithm(std::string& expr, stack_t operands, stack_t operators, vec_t sortedExpr) {
    addMissingMultiplication(expr);
    solveMultipleMinuses(expr);

    if (expr[0] == '-') {
        expr.replace(0, 1, 1, 'q');
    }

    for (int i = 0; i < expr.length(); i++) {
        if (isDigit(expr, i)) {
            lexNumbers(expr, i, operands, operators);
        }
        else if (compareCharToList(expr[i], { '+', '-' })) {
            lexAdditionSubtraction(expr, i, operands, operators);
        }
        else if (compareCharToList(expr[i], { '*', '/', '%' })) {
            lexMultiplicationDivisionModulus(expr, i, operands, operators);
        }
        else if (expr[i] == '(') {
            lexBrackets('(', expr, i, operands, operators);
        }
        else if (expr[i] == ')') {
            lexBrackets(')', expr, i, operands, operators);
        }
        else if (expr[i] == '^') {
            lexPow(expr, i, operands, operators);
        }
        else if (isSquareCubicRoot(expr, i) || isSquareCubicRoot(expr, i + 1) && expr[i] == 'q') {
            lexSqrtCbrt(expr, i, operands, operators);
        }
        else if (isFunction(expr, i) || isFunction(expr, i + 1) && expr[i] == 'q') {
            lexFunction(expr, i, operands, operators);
        }
    }
    while (operators.size() != 0) {
        operands.push(operators.top());
        operators.pop();
    }
}

void sortExpr(stack_t operands, vec_t sortedExpr) {
    while (operands.size() != 0) {
        sortedExpr.push_back(operands.top());
        operands.pop();
    }
    std::reverse(sortedExpr.begin(), sortedExpr.end());

    for (int i = 0; i < sortedExpr.size(); i++) {
        if (sortedExpr[i] == "(" || sortedExpr[i] == ")") {
            sortedExpr.erase(sortedExpr.begin() + i);
        }
    }
}

void performUnaryOperation(vec_t sortedExpr, int& i, const std::function<double(double)>& unaryOperation, bool isNegative = false) {
    double result = unaryOperation(std::stod(sortedExpr[i]));
    sortedExpr[i] = std::to_string(isNegative ? -result : result);
    sortedExpr.erase(sortedExpr.begin() + (i + 1));

    i = 0;
}

void performBinaryOperation(vec_t sortedExpr, int& i, const std::function<double(double, double)>& binaryOperation) {
    sortedExpr[i] = std::to_string(binaryOperation(std::stod(sortedExpr[i]), std::stod(sortedExpr[i + 1])));
    sortedExpr.erase(sortedExpr.begin() + (i + 1));
    sortedExpr.erase(sortedExpr.begin() + (i + 1));

    i = 0;
}

void _calculateExpr(vec_t sortedExpr, int i) {
    static const std::map<std::string, std::function<double(double)>> unaryOperations = {
        {"sqrt", [](double x) { return std::sqrt(x); }},
        {"cbrt", [](double x) { return std::cbrt(x); }},

        {"sin", [](double x) { return std::sin(x); }},
        {"cos", [](double x) { return std::cos(x); }},
        {"tan", [](double x) { return std::tan(x); }},
        {"cot", [](double x) { return 1.00 / std::tan(x); }},
        {"sec", [](double x) { return 1.00 / std::cos(x); }},
        {"csc", [](double x) { return 1.00 / std::sin(x); }},

        {"ln", [](double x) { return std::log(x); }},
        {"log", [](double x) { return std::log10(x); }},
        {"log2", [](double x) {return std::log2(x); }},
        {"exp", [](double x) { return std::exp(x); }},

        {"sinh", [](double x) { return std::sinh(x); }},
        {"cosh", [](double x) { return std::cosh(x); }},
        {"tanh", [](double x) { return std::tanh(x); }},
        {"coth", [](double x) { return 1.00 / std::tanh(x); }},
        {"sech", [](double x) { return 1.00 / std::cosh(x); }},
        {"csch", [](double x) { return 1.00 / std::sinh(x); }},

        {"asinh", [](double x) { return std::asinh(x); }},
        {"acosh", [](double x) { return std::acosh(x); }},
        {"atanh", [](double x) { return std::atanh(x); }},
        {"acoth", [](double x) { return 1.00 / std::atanh(x); }},
        {"asech", [](double x) { return 1.00 / std::acosh(x); }},
        {"acsch", [](double x) { return 1.00 / std::asinh(x); }}
    };

    static const std::map<std::string, std::function<double(double, double)>> binaryOperations = {
        {"+", std::plus<double>()},
        {"-", std::minus<double>()},
        {"*", std::multiplies<double>()},
        {"/", std::divides<double>()},
        {"%", [](double x, double y) { return std::fmod(x, y); }},
        {"^", [](double base, double exponent) { return std::pow(base, exponent); }},
    };

    bool hasPerformedOperation = false;
    std::string function;
    bool isNegative = false;
    if (sortedExpr[i + 1].length() > 1 && sortedExpr[i + 1][0] == '-') {
        function = sortedExpr[i + 1].substr(1);
        isNegative = true;
    }
    else {
        function = sortedExpr[i + 1];
    }
    for (const auto& unary : unaryOperations) {
        if (function == unary.first) {
            performUnaryOperation(sortedExpr, i, unary.second, isNegative);
            hasPerformedOperation = true;
            break;
        }
    }

    if (!hasPerformedOperation) {
        for (const auto& binary : binaryOperations) {
            if (sortedExpr[i + 2] == binary.first) {
                performBinaryOperation(sortedExpr, i, binary.second);
                hasPerformedOperation = true;
                break;
            }
        }
    }
    if (!hasPerformedOperation) {
        i++;
    }

    while (sortedExpr.size() >= 3) {
        _calculateExpr(sortedExpr, i);
    }
}

void calculateExpr(vec_t sortedExpr) {
    _calculateExpr(sortedExpr, 0);
}

void eraseUnnecessaryDecimals(vec_t sortedExpr) {
    std::string& expr = sortedExpr[0];

    size_t decimalPos = expr.find('.');
    if (decimalPos != std::string::npos) {
        while (expr.back() == '0' && expr.size() > decimalPos + 1) {
            if (expr[expr.size() - 2] == '.') {
                expr.pop_back();
                break;
            }
            expr.pop_back();
        }
    }
}

namespace seval {
    bool checkSyntax(std::string expr) {
        if (expr.empty() || isBinaryOpNoMinus(expr, 0)) {
            return false;
        }

        for (int i = 0; i < expr.length() - 1; i++) {
            if (isBinaryOp(expr, i) && isBinaryOp(expr, i + 1)) {
                return false;
            }
        }

        size_t leftParenCounter = 0, rightParenCounter = 0;
        for (int i = 0; i < expr.length(); i++) {
            if (isBinaryOp(expr, i) && isBinaryOp(expr, i + 1)) {
                return false;
            }
            else if (isSubstring(expr, i, "()") 
                 || isSubstring(expr, i, "(-)") 
                 || expr[i] == '(' && isBinaryOpNoMinus(expr, i + 1)
                 || isBinaryOp(expr, i) && expr[i + 1] == ')') {
                return false;
            }
            else if (expr[i] == '(') {
                leftParenCounter++;
            }
            else if (expr[i] == ')') {
                rightParenCounter++;
            }
        }
        if (leftParenCounter != rightParenCounter || isBinaryOp(expr, static_cast<int>(expr.length() - 1))) {
            return false;
        }
        return true;
    }
}

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    std::optional<double> seval::evalExpr(std::string expr) {
        eraseWhiteSpaces(expr);

        std::optional<double> result;
        if (expr.empty() || !seval::checkSyntax(expr)) {
            return result;
        }

        std::vector<std::string> sortedExpr;
        if (isPointlessExpr(expr)) {
            solveMultipleMinuses(expr);
            eraseBrackets(expr);
            sortedExpr.push_back(expr);
            return std::stod(sortedExpr[0]);
        }

        std::stack<std::string> operands, operators;
        shuntingYardAlgorithm(expr, operands, operators, sortedExpr);
        sortExpr(operands, sortedExpr);
        calculateExpr(sortedExpr);
        eraseUnnecessaryDecimals(sortedExpr);

        return std::stod(sortedExpr[0]);
    }
#else
    double seval::evalExpr(std::string expr) {
        eraseWhiteSpaces(expr);

        std::vector<std::string> sortedExpr;
        if (isPointlessExpr(expr)) {
            solveMultipleMinuses(expr);
            eraseBrackets(expr);
            sortedExpr.push_back(expr);
            return std::stod(sortedExpr[0]);
        }

        std::stack<std::string> operands, operators;
        shuntingYardAlgorithm(expr, operands, operators, sortedExpr);
        sortExpr(operands, sortedExpr);
        calculateExpr(sortedExpr);
        eraseUnnecessaryDecimals(sortedExpr);

        return std::stod(sortedExpr[0]);
    }
#endif
