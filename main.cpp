#include <iostream>
#include "simple-evaluator.h"

int main() {
	std::string expression = "asinh(10)";
	auto evaluatedExpression = seval::evalExpr(expression);
	if (evaluatedExpression) {
		std::cout << "The result is: " << evaluatedExpression.value();
	}
	else {
		std::cout << "Invalid syntax";
	}
}
