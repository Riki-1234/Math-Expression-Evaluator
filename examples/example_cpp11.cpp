#include <iostream>
#include "simple-evaluator.h"

int main() {
    std::string expression = "2*5 + 3*(4+4)/10";
    if (seval::checkSyntax(expression)) {
        std::cout << "The result is: " << seval::evalExpr(expression);
    }
    else {
        std::cout << "Invalid syntax";
    }
}
