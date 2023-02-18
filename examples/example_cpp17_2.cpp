#include <iostream>
#include "simple-evaluator.h"

int main() {
	auto evaluatedExpression = seval::evalExpr("5(5+5) * cos(10)");
	if (evaluatedExpression) {
		std::cout << "The result is: " << seval::evalExpr(expression).value();
	}
	else {
		std::cout << "Invalid syntax";
	}
}
