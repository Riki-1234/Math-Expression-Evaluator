#include <iostream>
#include "simple-evaluator.h"

int main() {
	std::string expression = "(sqrt(3) + (4 * 2)) / (5 - 1)";
	auto evaluatedExpression = seval::evalExpr(expression);
	if (evaluatedExpression) {
		std::cout << "The result is: " << evaluatedExpression.value();
	}
	else {
		std::cout << "Invalid syntax";
	}
}
