#include <cerrno>
#include <cfenv>
#include <cstring>

#include <iostream>

#include "expression_tree/expression.hpp"
#include "expression_tree/operators.hpp"
#include "expression_tree/functions.hpp"

/// @brief Equivalent to `expr.eval()` but implemented using `dynamic_cast` instead of virtual
/// functions
/// @pre expr.isComlete()
static double eval(const Expression& expr) {
	if (const Number* number = dynamic_cast<const Number*>(&expr)) {
		return number->eval();
	}

	if (const Negation* operation = dynamic_cast<const Negation*>(&expr)) {
		return -eval(*operation->first());
	}

	if (const Addition* operation = dynamic_cast<const Addition*>(&expr)) {
		return eval(*operation->first()) + eval(*operation->second());
	}

	if (const Subtraction* operation = dynamic_cast<const Subtraction*>(&expr)) {
		return eval(*operation->first()) - eval(*operation->second());
	}

	if (const Multiplication* operation = dynamic_cast<const Multiplication*>(&expr)) {
		return eval(*operation->first()) * eval(*operation->second());
	}

	if (const Division* operation = dynamic_cast<const Division*>(&expr)) {
		return eval(*operation->first()) / eval(*operation->second());
	}

	if (const Sin* func = dynamic_cast<const Sin*>(&expr)) {
		return std::sin(eval(*func->first()));
	}

	if (const Cos* func = dynamic_cast<const Cos*>(&expr)) {
		return std::cos(eval(*func->first()));
	}

	if (const Sqrt* func = dynamic_cast<const Sqrt*>(&expr)) {
		return std::sqrt(eval(*func->first()));
	}

	if (const Pow* func = dynamic_cast<const Pow*>(&expr)) {
		return std::pow(eval(*func->first()), eval(*func->second()));
	}

	return std::nan("0");
}

static void printNpn(const Expression& expr, std::ostream& output) {
	expr.printToken(output);

	const size_t count = expr.arity();
	if (count > 0)
		output << " (";
	for (size_t i = 0; i < count; ++i) {
		if (i != 0)
			output << " ";
		if (const Expression* child = expr.child(i)) {
			printNpn(*child, output);
		}
		else {
			output << '#';
		}
	}
	if (count > 0)
		output << ')';
}

static void printRpn(const Expression& expr, std::ostream& output) {
	const size_t count = expr.arity();
	if (count > 0)
		output << '(';
	for (size_t i = 0; i < count; ++i) {
		if (i != 0)
			output << " ";
		if (const Expression* child = expr.child(i)) {
			printRpn(*child, output);
		}
		else {
			output << '#';
		}
	}
	if (count > 0)
		output << ") ";

	expr.printToken(output);
}

static void printEvalResultChecked(const Expression& expr) {
	if (!expr.isComplete()) {
		std::cout << "Error: Invalid expression.\n";
		return;
	}
	errno = 0;
	std::feclearexcept(FE_ALL_EXCEPT);
	std::cout << "Result (virtual method): " << expr.eval() << "\n";
	std::cout << "Result (free function): " << eval(expr) << "\n";
	const int feFlags = std::fetestexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW);
	if (feFlags) {
		std::cout << "Numerical error(s) detected:";
		if (feFlags & FE_INVALID) {
			std::cout << " Domain error. ";
		}
		if (feFlags & FE_DIVBYZERO) {
			std::cout << " The result is undefined or infinite.";
		}
		if (feFlags & FE_OVERFLOW) {
			std::cout << " Overflow.";
		}
		if (feFlags & FE_UNDERFLOW) {
			std::cout << " Underflow.";
		}
		std::cout << "\n";
	}
}

static void testExpression(const Expression& expr) {
	std::cout << "Infix notation: ";
	expr.printInfixRecursive(std::cout);
	std::cout << '\n';

	std::cout << "Normal Polish notation: ";
	printNpn(expr, std::cout);
	std::cout << '\n';

	std::cout << "Reverse Polish notation: ";
	printRpn(expr, std::cout);
	std::cout << '\n';

	printEvalResultChecked(expr);
}

int main() {
	std::unique_ptr<Expression> exprCloned;

	{
		std::cout << "Testing \"3 + (5 + 9) * 2\":\n";
		std::unique_ptr<Expression> expr1 =
			std::make_unique<Addition>(
				std::make_unique<Number>(3),
				std::make_unique<Multiplication>(
					std::make_unique<Addition>(
						std::make_unique<Number>(5),
						std::make_unique<Number>(9)
					),
					std::make_unique<Number>(2)
				)
			);
		testExpression(*expr1);
		exprCloned = expr1->clone();
	}
	{
		std::cout << "\nTesting cloning:\n";
		testExpression(*exprCloned);
	}
	{
		std::cout << "\nTesting functions:\n";
		std::unique_ptr<Expression> expr2 =
			std::make_unique<Cos>(
				std::make_unique<Multiplication>(
					std::make_unique<Pow>(
						std::make_unique<Sqrt>(std::make_unique<Number>(81)),
						std::make_unique<Number>(0.5)
					),
					std::make_unique<Negation>(std::make_unique<Number>(pi))
				)
			);
		testExpression(*expr2);
	}
	{
		std::cout << "\nTesting division by zero:\n";
		std::unique_ptr<Expression> expr3 =
			std::make_unique<Division>(
				std::make_unique<Number>(5),
				std::make_unique<Number>(0)
			);
		testExpression(*expr3);
	}
}
