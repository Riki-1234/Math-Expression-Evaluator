# Math Expression Evaluator Library

This library can be used for evaluating math expressions which are in form of strings. The usage is different for C++11 & C++14 compared to C++17 as optionals are available in that version. in the C++17 version the evalExpr function both evaluates the expression and checks for the syntax but in the C++11 & C++14 version you have to check it manually via the checkSyntax function as you might get the wrong return type.

The namespace for all versions is seval::

To understand how to use the functions listed below, open up the examples folder on the github repository.

## C++11 & C++14

**`bool checkSyntax(std::string expression)`**

**`double evalExpr(std::string expression)`**

## C++17 & above

**`std::optional<double> evalExpr(std::string expression)`**

## Math Operators & Functions
 ## `+/-`
 ## `+`
 ## `-`
 ## `*`
 ## `/`
 ## `%`
 ## `()`
 ## `sqrt`
 ## `cbrt`
 ## `sin`
 ## `cos`
 ## `tan`
