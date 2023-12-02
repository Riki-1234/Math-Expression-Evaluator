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

bool isTopOperatorEqualTo(const stack_t operators, const std::initializer_list<std::string>& operatorList) {
    if (!operators.empty()) {
        for (const auto& op : operatorList) {
            if (operators.top() == op) {
                return true;
            }
        }
    }
    return false;
}

bool compareCharToList(char ch, const std::initializer_list<char>& list) {
    for (const auto& str : list) {
        if (ch == str) {
            return true;
        }
    }
    return false;
}

bool isSubstring(const std::string& expr, int i, const std::string& pattern) {
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

bool isSinCosTan(const std::string& expr, int i) {
    return (isSubstring(expr, i, "si") || isSubstring(expr, i, "co") || isSubstring(expr, i, "ta"));
}

bool isTopHighPrecedenceOperator(const stack_t operators) {
    return isTopOperatorEqualTo(operators, { "sqrt", "cbrt", "sin", "cos", "tan", "^" });
}

bool isDigit(const std::string& expr, int i) {
    return (std::isdigit(expr[i]) || expr[i] == 'n' && !isSinCosTan(expr, i + 1) && !isSquareCubicRoot(expr, i + 1) || expr[i] == '.');
}

bool isAdditionMultiplicationDivision(const std::string& expr, int i) {
    return compareCharToList(expr[i], { '+', '*', '/' });
}

bool isBinaryOp(const std::string& expr, int i) {
    return compareCharToList(expr[i], { '+', '-', '*', '/', '^', '%' });
}

bool isBinaryOpNoPercent(const std::string& expr, int i) {
    return compareCharToList(expr[i], { '+', '-', '*', '/', '^' });
}

bool isBinaryOpNoMinus(const std::string& expr, int i) {
    return compareCharToList(expr[i], { '+', '*', '/', '^', '%' });
}

bool isMissingMultiplication(const std::string& expr, int i) {
    return (std::isdigit(expr[i]) && expr[i + 1] == '('
        || expr[i] == ')' && std::isdigit(expr[i + 1])
        || isSubstring(expr, i, ")(")
        || isSubstring(expr, i, "%("));
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
        if (isBinaryOp(expr, i) || isSquareCubicRoot(expr, i) || isSinCosTan(expr, i)) {
            operatorCounter++;

            return false;
        }
    }
    return true;
}

void lexMultiplicationAndDivision(const std::string& expr, int i, stack_t operands, stack_t operators) {
    if (isTopHighPrecedenceOperator(operators) || isTopOperatorEqualTo(operators, { "*", "/" })) {
        operands.push(operators.top());
        operators.pop();
    }
    operators.push(std::string(1, expr[i]));
}

void lexNumbers(std::string& expr, int& i, stack_t operands, stack_t operators) {
    std::string multiDigitNum{};
    if (std::isdigit(expr[i + 1]) || expr[i] == 'n' && !isSinCosTan(expr, i + 1) && !isSquareCubicRoot(expr, i + 1) || expr[i + 1] == '.') {
        while (isDigit(expr, i)) {
            if (expr[i] == 'n') {
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

void lexAdditionAndSubtraction(const std::string& expr, int i, stack_t operands, stack_t operators) {
    if (isTopOperatorEqualTo(operators, { "*", "/" }) || isTopHighPrecedenceOperator(operators)) {
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

void lexSinCosTan(std::string& expr, int& i, stack_t& operands, stack_t& operators) {
    if (isTopHighPrecedenceOperator(operators)) {
        operands.push(operators.top());
        operators.pop();
        operators.push(expr.substr(i, 3));
    }
    else if (expr[i] == 'n') {
        expr.replace(i, 1, 1, '-');
        operators.push(expr.substr(i, 4));
    }
    else if (i > 0) {
        if (expr[i - 1] != '-') {
            operators.push(expr.substr(i, 3));
        }
    }
    else if (i == 0) {
        operators.push(expr.substr(i, 3));
    }
    i += 2;
}

void lexSqrtCbrt(std::string& expr, int& i, stack_t& operands, stack_t& operators) {
    if (isTopHighPrecedenceOperator(operators)) {
        operands.push(operators.top());
        operators.pop();
        operators.push(expr.substr(i, 4));
    }
    else if (expr[i] == 'n') {
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
            expr.replace(i + 1, 1, 1, 'n');
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

void replacePercentWithDivision(std::string& expr) {
    size_t percentIndex = expr.find('%');
    while (percentIndex != std::string::npos) {
        expr.replace(percentIndex, 1, "/100");

        percentIndex = expr.find('%', percentIndex + 1);
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
    expr.erase(std::remove_if(expr.begin(), expr.end(), [](char c) { return c == ' '; }), expr.end());
}

void eraseBrackets(std::string& expr) {
    expr.erase(std::remove_if(expr.begin(), expr.end(), [](char c) { return c == '(' || c == ')'; }), expr.end());
}

void shuntingYardAlgorithm(std::string& expr, stack_t operands, stack_t operators, vec_t sortedExpr) {
    replacePercentWithDivision(expr);
    addMissingMultiplication(expr);
    solveMultipleMinuses(expr);

    if (expr[0] == '-') {
        expr.replace(0, 1, 1, 'n');
    }

    for (int i = 0; i < expr.length(); i++) {
        if (isDigit(expr, i)) {
            lexNumbers(expr, i, operands, operators);
        }
        else if (compareCharToList(expr[i], {'+', '-'})) {
            lexAdditionAndSubtraction(expr, i, operands, operators);
        }
        else if (compareCharToList(expr[i], { '*', '/' })) {
            lexMultiplicationAndDivision(expr, i, operands, operators);
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
        else if (isSquareCubicRoot(expr, i) || isSquareCubicRoot(expr, i + 1) && expr[i] == 'n') {
            lexSqrtCbrt(expr, i, operands, operators);
        }
        else if (isSinCosTan(expr, i) || isSinCosTan(expr, i + 1) && expr[i] == 'n') {
            lexSinCosTan(expr, i, operands, operators);
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

void performUnaryOperation(vec_t sortedExpr, int& i, const std::function<double(double)>& unaryOperation) {
    sortedExpr[i] = std::to_string(unaryOperation(std::stod(sortedExpr[i])));
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
        {"-sqrt", [](double x) { return -std::sqrt(x); }},
        {"cbrt", [](double x) { return std::cbrt(x); }},
        {"-cbrt", [](double x) { return -std::cbrt(x); }},
        {"sin", [](double x) { return std::sin(x); }},
        {"-sin", [](double x) { return -std::sin(x); }},
        {"cos", [](double x) { return std::cos(x); }},
        {"-cos", [](double x) { return -std::cos(x); }},
        {"tan", [](double x) { return std::tan(x); }},
        {"-tan", [](double x) { return -std::tan(x); }}
    };

    static const std::map<std::string, std::function<double(double, double)>> binaryOperations = {
        {"+", std::plus<double>()},
        {"-", std::minus<double>()},
        {"*", std::multiplies<double>()},
        {"/", std::divides<double>()},
        {"^", [](double base, double exponent) { return std::pow(base, exponent); }}
    };

    bool hasPerformedOperation = false;
    for (const auto& binary : binaryOperations) {
        if (sortedExpr[i + 2] == binary.first) {
            performBinaryOperation(sortedExpr, i, binary.second);
            hasPerformedOperation = true;
            break;
        }
    }
    if (!hasPerformedOperation) {
        for (const auto& unary : unaryOperations) {
            if (sortedExpr[i + 1] == unary.first) {
                performUnaryOperation(sortedExpr, i, unary.second);
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
            if (isBinaryOpNoPercent(expr, i) && isBinaryOpNoPercent(expr, i + 1)) {
                return false;
            }
            else if (isBinaryOpNoPercent(expr, i) && expr[i + 1] == '%') {
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
        if (leftParenCounter != rightParenCounter || isBinaryOpNoPercent(expr, static_cast<int>(expr.length() - 1))) {
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
