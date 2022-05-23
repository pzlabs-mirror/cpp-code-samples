#ifndef OPERATORS_HPP_INCLUDED
#define OPERATORS_HPP_INCLUDED

#include "expression_tree/expression.hpp"

class Negation final: public UnaryOperator {
public:
	using UnaryOperator::UnaryOperator;
	using UnaryExpression::child;

	bool isPrefix() const override { return true; }

	double eval() const override {
		return -m_first->eval();
	}

	int precedence() const override { return 12; }

	void printToken(std::ostream& output) const override {
		output << '-';
	}

	std::unique_ptr<Expression> clone() const override {
		return std::make_unique<Negation>(cloneOrNull(m_first.get()));
	}
};

class Addition final: public BinaryOperator {
public:
	using BinaryOperator::BinaryOperator;

	double eval() const override {
		return m_first->eval() + m_second->eval();
	}

	int precedence() const override { return 8; }

	void printToken(std::ostream& output) const override {
		output << '+';
	}

	std::unique_ptr<Expression> clone() const override {
		return std::make_unique<Addition>(cloneOrNull(m_first.get()), cloneOrNull(m_second.get()));
	}
};

class Subtraction final: public BinaryOperator {
public:
	using BinaryOperator::BinaryOperator;

	double eval() const override {
		return m_first->eval() - m_second->eval();
	}

	int precedence() const override { return 8; }

	void printToken(std::ostream& output) const override {
		output << '-';
	}

	std::unique_ptr<Expression> clone() const override {
		return std::make_unique<Subtraction>(cloneOrNull(m_first.get()),
			cloneOrNull(m_second.get()));
	}
};

class Multiplication final: public BinaryOperator {
public:
	using BinaryOperator::BinaryOperator;

	double eval() const override {
		return isComplete() ? m_first->eval() * m_second->eval() : std::nan("*");
	}

	int precedence() const override { return 10; }

	void printToken(std::ostream& output) const override {
		output << '*';
	}

	std::unique_ptr<Expression> clone() const override {
		return std::make_unique<Multiplication>(cloneOrNull(m_first.get()),
			cloneOrNull(m_second.get()));
	}
};

class Division final: public BinaryOperator {
public:
	using BinaryOperator::BinaryOperator;

	double eval() const override {
		return m_first->eval() / m_second->eval();
	}

	int precedence() const override { return 10; }

	void printToken(std::ostream& output) const override {
		output << '/';
	}

	std::unique_ptr<Expression> clone() const override {
		return std::make_unique<Division>(cloneOrNull(m_first.get()),
			cloneOrNull(m_second.get()));
	}
};

#endif
