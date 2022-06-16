#ifndef FUNCTIONS_HPP_INCLUDED
#define FUNCTIONS_HPP_INCLUDED

#include <cmath>

#include "expression_tree/expression.hpp"

inline constexpr double pi = 3.141592653589793238462643383279502884;

class Sin final: public UnaryFunction {
public:
	using UnaryFunction::UnaryFunction;

	double eval() const override { return std::sin(m_first->eval()); }

	void printToken(std::ostream& output) const override {
		output << "sin";
	}

	std::unique_ptr<Expression> clone() const override {
		return std::make_unique<Sin>(cloneOrNull(m_first.get()));
	}
};

class Cos final: public UnaryFunction {
public:
	using UnaryFunction::UnaryFunction;

	double eval() const override { return std::cos(m_first->eval()); }

	void printToken(std::ostream& output) const override {
		output << "cos";
	}

	std::unique_ptr<Expression> clone() const override {
		return std::make_unique<Cos>(cloneOrNull(m_first.get()));
	}
};

class Sqrt final: public UnaryFunction {
public:
	using UnaryFunction::UnaryFunction;

	double eval() const override { return std::sqrt(m_first->eval()); }

	void printToken(std::ostream& output) const override {
		output << "sqrt";
	}

	std::unique_ptr<Expression> clone() const override {
		return std::make_unique<Sqrt>(cloneOrNull(m_first.get()));
	}
};

class Pow final: public BinaryFunction {
public:
	using BinaryFunction::BinaryFunction;

	double eval() const override { return std::pow(m_first->eval(), m_second->eval()); }

	void printToken(std::ostream& output) const override {
		output << "pow";
	}

	std::unique_ptr<Expression> clone() const override {
		return std::make_unique<Sqrt>(cloneOrNull(m_first.get()));
	}
};

#endif
