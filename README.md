# Math Expression Evaluator Library

This library can be used for evaluating math expressions which are in form of strings. It works with C++11 and above but the way it's used for different versions isn't the same and the return type isn't the same. So here I will explain every detail about it.

The namespace for all versions is **seval::**

To understand how to use the functions listed below, open up the examples folder on the github repository.

## C++11 & C++14

**Functions:**

**bool checkSyntax(std::string expression)**

**double evalExpr(std::string expression)**

## C++17 & above

**Functions:**

**std::optional`<double>` evalExpr(std::string expression)**

## Math Operators & Functions
 ## `+`
 ## `-`
 ## `*`
 ## `/`
 ## `%`
 ## `sqrt`
 ## `cbrt`
 ## `sin`
 ## `cos`
 ## `tan`
