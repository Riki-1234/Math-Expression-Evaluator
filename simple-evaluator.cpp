#include "simple-evaluator.h"
#include <vector>
#include <stack>
#include <utility>
#include <cmath>
#include <algorithm>

bool isSquareCubicRoot(const std::string& expression, int i) {
    return (expression[i] == 's' && expression[i + 1] == 'q' || expression[i] == 'c' && expression[i + 1] == 'b');
}

bool isSinCosTan(const std::string& expression, int i) {
    return (expression[i] == 's' && expression[i + 1] == 'i' || expression[i] == 'c' && expression[i + 1] == 'o' || expression[i] == 't' && expression[i + 1] == 'a');
}

bool isTopOperator(const std::stack<std::string>& operators) {
    return (operators.top() == "^" || operators.top() == "sqrt" || operators.top() == "cbrt" || operators.top() == "sin" || operators.top() == "cos" || operators.top() == "tan");
}

bool isTopHighPrecedenceOperator(const std::stack<std::string>& operators) {
    if (operators.size() > 0) {
        return (operators.top() == "sqrt" || operators.top() == "cbrt" || operators.top() == "sin" || operators.top() == "cos" || operators.top() == "tan" || operators.top() == "^");
    }
    return false;
}

bool isDigit(const std::string& expression, int i) {
    return (std::isdigit(expression[i]) || expression[i] == 'n' && !isSinCosTan(expression, i + 1) && !isSquareCubicRoot(expression, i + 1) || expression[i] == '.');
}

bool isAdditionMultiplicationDivision(const std::string& expression, int i) {
    return (expression[i] == '+' || expression[i] == '*' || expression[i] == '/');
}

bool isOperator(const std::string& expression, int i) {
    return (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/' || expression[i] == '^' || expression[i] == '%');
}

bool isOperatorNoPercent(const std::string& expression, int i) {
    return (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/' || expression[i] == '^');
}

bool isOperatorNoMinus(const std::string& expression, int i) {
    return (expression[i] == '+' || expression[i] == '*' || expression[i] == '/' || expression[i] == '^' || expression[i] == '%');
}

bool isMissingMultiplication(const std::string& expression, int i) {
    return (std::isdigit(expression[i]) && expression[i + 1] == '('
        || expression[i] == ')' && std::isdigit(expression[i + 1])
        || expression[i] == ')' && expression[i + 1] == '('
        || expression[i] == '%' && expression[i + 1] == '(');
}

bool isPointlessExpression(const std::string& expression) {
    int operatorCounter = 0;
    for (int i = 0; i < expression.length(); i++) {
        if (i != expression.length() - 1) {
            if (expression[i] == '(' && expression[i + 1] == '-') {
                if (i == 0) {
                    i++;
                }
                i++;
                continue;
            }
        }
        if (isOperator(expression, i) || isSquareCubicRoot(expression, i) || isSinCosTan(expression, i)) {
            operatorCounter++;
        }
    }
    if (operatorCounter > 0) {
        return false;
    }
    return true;
}

void lexMultiplicationAndDivision(const std::string& expression, int i, std::stack<std::string>& operands, std::stack<std::string>& operators) {
    if (operators.size() > 0) {
        if (isTopOperator(operators) || operators.top() == "*" || operators.top() == "/") {
            operands.push(operators.top());
            operators.pop();
            operators.push(std::string(1, expression[i]));
        }
        else {
            operators.push(std::string(1, expression[i]));
        }
    }
    else {
        operators.push(std::string(1, expression[i]));
    }
}

void lexNumbers(std::string& expression, int& i, std::stack<std::string>& operands, std::stack<std::string>& operators) {
    std::string multiDigitNum{};
    if (std::isdigit(expression[i + 1]) || expression[i] == 'n' && !isSinCosTan(expression, i + 1) && !isSquareCubicRoot(expression, i + 1) || expression[i + 1] == '.') {
        while (isDigit(expression, i)) {
            if (expression[i] == 'n') {
                expression.replace(i, 1, 1, '-');
            }
            multiDigitNum += std::string(1, expression[i]);
            i++;
        }
        operands.push(multiDigitNum);
        i--;
    }
    else {
        operands.push(std::string(1, expression[i]));
    }
}

void lexAdditionAndSubtraction(const std::string& expression, int i, std::stack<std::string>& operands, std::stack<std::string>& operators) {
    if (operators.empty()) {
        operators.push(std::string(1, expression[i]));
    }
    else if (operators.top() == "*" || operators.top() == "/" || isTopHighPrecedenceOperator(operators)) {
        while (operators.size() != 0) {
            operands.push(operators.top());
            operators.pop();
        }
        operators.push(std::string(1, expression[i]));
    }
    else if (operators.top() == "+" || operators.top() == "-") {
        operands.push(operators.top());
        operators.pop();
        operators.push(std::string(1, expression[i]));
    }
    else {
        operators.push(std::string(1, expression[i]));
    }
}

void lexSinCosTan(std::string& expression, int& i, std::stack<std::string>& operands, std::stack<std::string>& operators) {
    if (isTopHighPrecedenceOperator(operators)) {
        operands.push(operators.top());
        operators.pop();
        operators.push(std::string() + expression[i] + expression[i + 1] + expression[i + 2]);
    }
    else if (expression[i] == 'n') {
        expression.replace(i, 1, 1, '-');

        operators.push(std::string() + expression[i] + expression[i + 1] + expression[i + 2] + expression[i + 3]);
    }
    else if (i > 0) {
        if (expression[i - 1] != '-') {
            operators.push(std::string() + expression[i] + expression[i + 1] + expression[i + 2]);
        }
    }
    else if (i == 0) {
        operators.push(std::string() + expression[i] + expression[i + 1] + expression[i + 2]);
    }
    i += 2;
}

void lexSqrtCbrt(std::string& expression, int& i, std::stack<std::string>& operands, std::stack<std::string>& operators) {
    if (isTopHighPrecedenceOperator(operators)) {
        operands.push(operators.top());
        operators.pop();
        operators.push(std::string() + expression[i] + expression[i + 1] + expression[i + 2] + expression[i + 3]);
    }
    else if (expression[i] == 'n') {
        expression.replace(i, 1, 1, '-');

        operators.push(std::string() + expression[i] + expression[i + 1] + expression[i + 2] + expression[i + 3] + expression[i + 4]);
    }
    else if (i > 0) {
        if (expression[i - 1] != '-') {
            operators.push(std::string() + expression[i] + expression[i + 1] + expression[i + 2] + expression[i + 3]);
        }
    }
    else if (i == 0) {
        operators.push(std::string() + expression[i] + expression[i + 1] + expression[i + 2] + expression[i + 3]);
    }
    i += 3;
}

void replacePercentWithDivision(std::string& expression) {
    while (expression.find("%") != std::string::npos) {
        int percentIndex{};
        for (int i = 0; i < expression.size(); i++) {
            if (expression[i] == '%') {
                percentIndex = i;
            }
        }

        expression.insert(expression.begin() + (percentIndex + 1), '1');
        expression.insert(expression.begin() + (percentIndex + 2), '0');
        expression.insert(expression.begin() + (percentIndex + 3), '0');

        expression.replace(expression.find("%"), 1, 1, '/');
    }
}

void addMissingMultiplication(std::string& expression) {
    for (int i = 0; i < expression.size(); i++) {
        if (isMissingMultiplication(expression, i)) {
            expression.insert(expression.begin() + (i + 1), '*');
        }
    }
}

void solveMultipleMinuses(std::string& expression) {
    int minusCounter = 0, minusIndex;
    for (int i = 0; i < expression.length(); i++) {
        if (expression[i] == '(' && expression[i + 1] == '-') {
            minusCounter++;

            expression.erase(expression.begin() + (i + 1));
            minusIndex = i + 1;
        }
        else if (i == 0 && expression[i] == '-' && expression[i + 1] == '(') {
            minusCounter++;

            expression.erase(expression.begin() + i);
            minusIndex = i;
            i--;
        }
        else if (expression[i] == '-' && expression[i + 1] == '(' && minusCounter != 0) {
            minusCounter++;

            expression.erase(expression.begin() + i);
            minusIndex = i;
        }
    }

    if (minusCounter % 2 != 0) {
        expression.insert(expression.begin() + minusIndex, '-');
    }
}

void eraseWhiteSpaces(std::string& expression) {
    for (int i = 0; i < expression.length(); i++) {
        if (expression[i] == ' ') {
            expression.erase(expression.begin() + i);
            if (i == 0) {
                i = -1;
            }
            else {
                i--;
            }
        }
    }
}

void eraseBrackets(std::string& expression) {
    for (int i = 0; i < expression.length(); i++) {
        if (expression[i] == '(' || expression[i] == ')') {
            expression.erase(expression.begin() + i);
            if (i == 1) {
                i = 0;
            }
            else {
                i--;
            }
        }
    }
}

void shuntingYardAlgorithm(std::string& expression, std::stack<std::string>& operands, std::stack<std::string>& operators, std::vector<std::string>& sortedExpression, bool& isPointless) {
    eraseWhiteSpaces(expression);
    if (isPointlessExpression(expression)) {
        solveMultipleMinuses(expression);
        eraseBrackets(expression);
        sortedExpression.push_back(expression);
        isPointless = true;
        return;
    }
    replacePercentWithDivision(expression);
    addMissingMultiplication(expression);
    solveMultipleMinuses(expression);

    if (expression[0] == '-') {
        expression.replace(0, 1, 1, 'n');
    }

    for (int i = 0; i < expression.length(); i++) {
        if (expression[i] == '+' || expression[i] == '-') {
            lexAdditionAndSubtraction(expression, i, operands, operators);
        }
        else if (expression[i] == '*' || expression[i] == '/') {
            lexMultiplicationAndDivision(expression, i, operands, operators);
        }
        else if (isDigit(expression, i)) {
            lexNumbers(expression, i, operands, operators);
        }
        else if (expression[i] == '(') {
            if (expression[i + 1] == '-') {
                expression.replace(i + 1, 1, 1, 'n');
            }
            operators.push(std::string(1, expression[i]));
        }
        else if (expression[i] == ')') {
            while (operators.top() != "(") {
                operands.push(operators.top());
                operators.pop();
            }
            operators.pop();
        }
        else if (expression[i] == '^') {
            if (isTopHighPrecedenceOperator(operators)) {
                operands.push(operators.top());
                operators.pop();
                operators.push(std::string(1, expression[i]));
            }
            else {
                operators.push(std::string(1, expression[i]));
            }
        }
        else if (isSquareCubicRoot(expression, i) || isSquareCubicRoot(expression, i + 1) && expression[i] == 'n') {
            lexSqrtCbrt(expression, i, operands, operators);
        }
        else if (isSinCosTan(expression, i) || isSinCosTan(expression, i + 1) && expression[i] == 'n') {
            lexSinCosTan(expression, i, operands, operators);
        }
    }
    while (operators.size() != 0) {
        operands.push(operators.top());
        operators.pop();
    }
}

void sortExpression(std::stack<std::string>& operands, std::vector<std::string>& sortedExpression) {
    while (operands.size() != 0) {
        sortedExpression.push_back(operands.top());
        operands.pop();
    }
    std::reverse(sortedExpression.begin(), sortedExpression.end());

    for (int i = 0; i < sortedExpression.size(); i++) {
        if (sortedExpression[i] == "(" || sortedExpression[i] == ")") {
            sortedExpression.erase(sortedExpression.begin() + i);
        }
    }
}

void _calculateExpression(std::vector<std::string>& sortedExpression, int i) {
    if (sortedExpression[i + 1] == "sqrt") {
        sortedExpression[i] = std::to_string(std::sqrt(std::stod(sortedExpression[i])));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 1] == "-sqrt") {
        sortedExpression[i] = std::to_string(std::sqrt(std::stod(sortedExpression[i])) * -1);
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 1] == "cbrt") {
        sortedExpression[i] = std::to_string(std::cbrt(std::stod(sortedExpression[i])));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 1] == "-cbrt") {
        sortedExpression[i] = std::to_string(std::cbrt(std::stod(sortedExpression[i])) * -1);
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 1] == "sin") {
        sortedExpression[i] = std::to_string(std::sin(std::stod(sortedExpression[i])));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 1] == "-sin") {
        sortedExpression[i] = std::to_string(std::sin(std::stod(sortedExpression[i])) * -1);
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 1] == "cos") {
        sortedExpression[i] = std::to_string(std::cos(std::stod(sortedExpression[i])));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 1] == "-cos") {
        sortedExpression[i] = std::to_string(std::cos(std::stod(sortedExpression[i])) * -1);
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 1] == "tan") {
        sortedExpression[i] = std::to_string(std::tan(std::stod(sortedExpression[i])));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 1] == "-tan") {
        sortedExpression[i] = std::to_string(std::tan(std::stod(sortedExpression[i])) * -1);
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 2] == "+") {
        sortedExpression[i] = std::to_string(std::stod(sortedExpression[i]) + std::stod(sortedExpression[i + 1]));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 2] == "-") {
        sortedExpression[i] = std::to_string(std::stod(sortedExpression[i]) - std::stod(sortedExpression[i + 1]));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 2] == "*") {
        sortedExpression[i] = std::to_string(std::stod(sortedExpression[i]) * std::stod(sortedExpression[i + 1]));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 2] == "/") {
        sortedExpression[i] = std::to_string(std::stod(sortedExpression[i]) / std::stod(sortedExpression[i + 1]));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else if (sortedExpression[i + 2] == "^") {
        sortedExpression[i] = std::to_string(std::pow(std::stod(sortedExpression[i]), std::stod(sortedExpression[i + 1])));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        sortedExpression.erase(sortedExpression.begin() + (i + 1));
        i = 0;
    }
    else {
        i++;
    }

    while (sortedExpression.size() >= 3) {
        _calculateExpression(sortedExpression, i);
    }
}

void calculateExpression(std::vector<std::string>& sortedExpression) {
    _calculateExpression(sortedExpression, 0);
}

void eraseUnnecessaryDecimals(std::vector<std::string>& sortedExpression) {
    std::string expression = sortedExpression[0];
    while (expression[expression.size() - 1] == '0' && expression.size() > 1 && expression.find('.') != std::string::npos) {
        if (expression[expression.size() - 2] == '.') {
            expression.erase(expression.size() - 2, 1);
        }

        expression.erase(expression.size() - 1, 1);
    }
    sortedExpression[0] = expression;
}

int digitCountFunc(std::string& expression, int& i) {
    int digitCount = 0;
    if (std::isdigit(expression[i + 1]) || expression[i] == 'n' || expression[i + 1] == '.') {
        while (isDigit(expression, i)) {
            i++;
        }
        digitCount++;
        i--;
    }
    else {
        digitCount++;
    }

    return digitCount;
}

namespace seval {
    bool checkSyntax(std::string expression) {
        if (expression.empty()) {
            return false;
        }
        if (isOperatorNoMinus(expression, 0)) {
            return false;
        }

        size_t leftParenCounter = 0, rightParenCounter = 0;
        for (int i = 0; i < expression.length(); i++) {
            if (isOperator(expression, i) && isOperator(expression, i + 1)) {
                return false;
            }
            else if (expression[i] == '(' && expression[i + 1] == ')'
                || expression[i] == '(' && expression[i + 1] == '-' && expression[i + 2] == ')'
                || expression[i] == '(' && isOperatorNoMinus(expression, i + 1)
                || isOperator(expression, i) && expression[i + 1] == ')') {
                return false;
            }
            else if (expression[i] == '(') {
                leftParenCounter++;
            }
            else if (expression[i] == ')') {
                rightParenCounter++;
            }
        }
        if (leftParenCounter != rightParenCounter || isOperatorNoPercent(expression, expression.length() - 1)) {
            return false;
        }
        return true;
    }
}
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    std::optional<double> seval::evalExpr(std::string expression) {
        std::optional<double> result;
        if (expression.empty()) {
            return result;
        }
        if (!seval::checkSyntax(expression)) {
            return result;
        }

        std::vector<std::string> sortedExpression;
        std::stack<std::string> operands, operators;
        bool isPointless = false;
        shuntingYardAlgorithm(expression, operands, operators, sortedExpression, isPointless);
        if (isPointless) {
            return std::stod(sortedExpression[0]);
        }
        sortExpression(operands, sortedExpression);
        calculateExpression(sortedExpression);
        eraseUnnecessaryDecimals(sortedExpression);

        result = std::stod(sortedExpression[0]);
        return result;
    }
#else
    double seval::evalExpr(std::string expression) {
        std::vector<std::string> sortedExpression;
        std::stack<std::string> operands, operators;
        bool isPointless = false;
        shuntingYardAlgorithm(expression, operands, operators, sortedExpression, isPointless);
        if (isPointless) {
            return std::stod(sortedExpression[0]);
        }
        sortExpression(operands, sortedExpression);
        calculateExpression(sortedExpression);
        eraseUnnecessaryDecimals(sortedExpression);

        return std::stod(sortedExpression[0]);
    }
#endif
