# Math Expression Evaluator Library

This library can be used for evaluating math expressions which are in form of strings.

### Keep in mind that the usage differs depending on the C++ version: 
#### Usage of C++11 & C++14 differs compared to C++17 as optionals are available in that version. 
#### The C++17 version has only the evalExpr function which both evaluates the expression and checks for the syntax.
#### The C++11 & C++14 version requires you to manually check the syntax via the checkSyntax function as you might get the wrong return value if the syntax of the passed expression is invalid.

The namespace for all versions is seval::

To understand how to use the functions listed below, open up the examples folder.

## C++11 & C++14

**`bool checkSyntax(std::string expression)`**

**`double evalExpr(std::string expression)`**

## C++17 & above

**`std::optional<double> evalExpr(std::string expression)`**

## Math Operators & Functions
 ### `+/-`
 ### `+`
 ### `-`
 ### `*`
 ### `/`
 ### `%`
 ### `()`
 ### `sqrt`
 ### `cbrt`
 ### `sin`
 ### `cos`
 ### `tan`
