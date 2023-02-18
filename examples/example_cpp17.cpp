#include <iostream>
#include "simple-evaluator.h"

int main() {
	std::string expression = "1+3*162/3^(5)+sqrt(9)*5";
	if (seval::evalExpr(expression).has_value()) {
		std::cout << "The result is: " << seval::evalExpr(expression).value();
	}
	else {
		std::cout << "Invalid syntax";
	}
}
