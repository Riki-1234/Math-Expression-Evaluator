# Math Expression Evaluator Library

This library can be used for evaluating math expressions which are in form of strings.

### Keep in mind that the usage differs depending on the C++ version: 
### C++11 & C++14:
#### In these versions you are required to manually check the syntax via the checkSyntax function as you might get the wrong return value if the syntax of the passed expression is invalid.
### C++17:
#### The C++17 version has only the evalExpr function which both evaluates the expression and checks for the syntax. <br><br>

#### The namespace for all versions is **seval**

#### To understand how to use the functions listed below, open up the examples folder.

## C++11 & C++14

**`bool checkSyntax(std::string expression)`**

**`double evalExpr(std::string expression)`**

## C++17 & above

**`std::optional<double> evalExpr(std::string expression)`**

## Math Operators & Functions

### Unary Plus And Negation Operators

 #### `+/-`

### Binary Operators
 
 #### `+`
 #### `-`
 #### `*`
 #### `/`
 #### `%`
 #### `^`

### Root Functions
 
 #### `sqrt`
 #### `cbrt`

### Trigonometric Functions
 
 #### `sin`
 #### `cos`
 #### `tan`
 #### `cot`
 #### `sec`
 #### `csc`

### Hyperbolic Functions
 
 #### `sinh`
 #### `cosh`
 #### `tanh`
 #### `coth`
 #### `sech`
 #### `csch`

### Inverse Hyperbolic Functions
 
 #### `asinh`
 #### `acosh`
 #### `atanh`
 #### `acoth`
 #### `asech`
 #### `acsch`

### Logarithmic Functions
 
 #### `ln`
 #### `log`
 #### `log2`
 #### `exp`
