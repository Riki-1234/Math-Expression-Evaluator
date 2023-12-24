#include <iostream>
#include <vector>
#include "simple-evaluator.h"

int main() {
	std::vector<std::pair<std::string, double>> expressions {
		{"-sin(5) + 5", 0.958924}
	};

    for (const auto& [expression, result] : expressions) {
        auto evaluatedExpression = seval::evalExpr(expression);
        if (evaluatedExpression) {
            std::cout << expression << " | Result: " << evaluatedExpression.value() << " | Expected: " << result << std::endl;
        }
        else {
            std::cout << "Invalid syntax: " << expression << std::endl;
        }
    }
}
