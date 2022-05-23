#ifndef EXPRESSION_HPP_INCLUDED
#define EXPRESSION_HPP_INCLUDED

#include <cmath>

#include <memory>

// UML diagram: https://www.plantuml.com/plantuml/png/pPRDRXCn483lVWfBI2W1vGMYgYejLE90gIYS4AeSxuIEyDgBVq9AoRlZ3HbYJIn86afpZ7T-ldpshFSaHELZQer06y5FbGRvQjvv206TbNT2okVoJar2z4h7XOIPCeFXM3OkJGpmf_e6JJD0sy1yB0D-X-kOOxMp8HPLd_4qvJ7U3eQKmXzZE7DjPo127pDnpl28N5b30rOl8z36pO2y-Dvz0JjmANOfvbwn6OzT3W3LFXrM4rxRASxVWKu-u0ospDJ6sOon2aiMloQuxg8_MWiu5WiXj54Xo8lKJi0lFOzaUvtj9lXjjozTCxw3vghRkj2wnItLxUPhQqd5KJpwCHgDLhw48D_oWrN-bftO9zda57s8Vwx_a2wNxViLNYD0F5y--plWi6g0_U52nIdUsyKgC81sjZb8QptOlZufuUfNDVgtEsy15osAb-VR3hoABe-qN6ncqjDYrywJrP5sgpQ4bJAKm-TWtNpteMIlt4iFEMz0kzfxeEhbGQGrmUUm5iEEwXwu6pIiek1RL8tY-aZhlNWekVpdj5QrmsAOdNzAmohMxV0ekgAka13uLSHlXxrVzoIaHC_jYUJCXSSaoUs9vD9z-ryafrX1oVq9vG8-g-AOqEJIUxKEMR_i7qGYcrF29VmNJOE4_qYa99cX4jfw4DAady_3f2tf2FXDCX4xWreT9ZC39EoNECDmcC249iJgmttqyGQRJFBfUFF3Z2qKiPjV4DId2A9RdYhLGggtNYsGNqVqgygrRYnEm3QfDJy1
// TODO: Associativity

/// @brief An algebraic expression tree
class Expression {
public:
	static constexpr int maxPrecedence = 20;

	///@note A polymorphic class should have a virtual destructor
	virtual ~Expression() = default;

	/// @brief Evaluate the expression and return the numerical result
	/// @pre `this->isComplete()`. The expression must be complete before evaluating
	/// @warning No error checking is performed. Make sure that the expression is fully
	/// constructed beforehand. Use floating point exceptions to detect numerical errors
	virtual double eval() const = 0;

	/// @brief Return a pointer to the child expression at specified location index or `nullptr`
	/// if no such child exists
	virtual const Expression* child(size_t index) const = 0;

	/// @brief Get the number of children
	virtual size_t arity() const = 0;

	/// @brief Check whether the expression is complete (i.e., all of its child nodes are non-null
	/// and complete)
	/// @note Recursive
	virtual bool isComplete() const = 0;

	/// @brief Get the precedence of the expression. The expression node with the higher precedence
	/// has higher priority in the infix notation unless parentheses explicitly change the order
	virtual int precedence() const = 0;

	/// @brief Write the 'name' of a single node as in the written notation to the specified output
	/// stream
	virtual void printToken(std::ostream& output) const = 0;

	/// @brief Write the infix notation of the expression including its child nodes to the specified
	/// output stream
	virtual void printInfixRecursive(std::ostream& output) const = 0;

	/// @brief Create a full copy of the expression tree
	virtual std::unique_ptr<Expression> clone() const = 0;

protected:
	// Ban constructing, copying and moving directly, but let the derived classes implement
	// their own special functions if possible
	Expression() = default;
	Expression(const Expression&) = default;
	Expression& operator=(const Expression&) = default;
	Expression(Expression&&) = default;
	Expression& operator=(Expression&&) = default;

	/// @brief Helper function to ease cloning incomplete expressions
	static std::unique_ptr<Expression> cloneOrNull(const Expression* expr) {
		return expr ? expr->clone() : std::unique_ptr<Expression>();
	}

	/// @brief Helper function to ease printing (in)complete expressions with or without
	/// parentheses
	static void printInfixRecursiveOrPlaceholder(const Expression* expr, std::ostream& output,
		bool withParentheses = false
	) {
		if (withParentheses) {
			output << '(';
		}

		if (expr) {
			expr->printInfixRecursive(output);
		}
		else {
			output << "#";
		}

		if (withParentheses) {
			output << ')';
		}
	}
};

class Number final: public Expression {
public:
	Number() = default;
	explicit Number(double value): m_value(value) { }

	double eval() const override {
		return m_value;
	}

	const Expression* child(size_t) const override {
		return nullptr;
	}

	size_t arity() const override {
		return 0;
	}

	bool isComplete() const override {
		return true;
	}

	int precedence() const override {
		return maxPrecedence;
	}

	void printToken(std::ostream& output) const override {
		output << m_value;
	}

	void printInfixRecursive(std::ostream& output) const override {
		output << m_value;
	}

	std::unique_ptr<Expression> clone() const override {
		return std::make_unique<Number>(*this);
	}

private:
	double m_value = std::nan("0");
};

class UnaryExpression: virtual public Expression {
public:
	UnaryExpression() = default;

	explicit UnaryExpression(std::unique_ptr<Expression> first):
		m_first(std::move(first))
	{ }

	const Expression* child(size_t index) const override final {
		return index == 0 ? m_first.get() : nullptr;
	}

	size_t arity() const override final {
		return 1;
	}

	bool isComplete() const override final {
		return m_first && m_first->isComplete();
	}

	const Expression* first() const { return m_first.get(); }
	Expression* first() { return m_first.get(); }

	void setFirst(std::unique_ptr<Expression> first) { m_first = std::move(first); }

protected:
	// `std::unique_ptr` automatically handles the lifetime of the child expression
	std::unique_ptr<Expression> m_first;
};

class BinaryExpression: virtual public Expression {
public:
	BinaryExpression() = default;

	BinaryExpression(std::unique_ptr<Expression> first, std::unique_ptr<Expression> second):
		m_first(std::move(first)),
		m_second(std::move(second))
	{ }

	const Expression* child(size_t index) const override final {
		return
			index == 0 ? m_first.get() :
			index == 1 ? m_second.get() :
			nullptr;
	}

	size_t arity() const override final {
		return 2;
	}

	bool isComplete() const override final {
		// Checking for both null pointers first is faster due to short-circuit evaluation
		return m_first && m_second && m_first->isComplete() && m_second->isComplete();
	}

	const Expression* first() const { return m_first.get(); }
	Expression* first() { return m_first.get(); }
	const Expression* second() const { return m_second.get(); }
	Expression* second() { return m_second.get(); }

	void setFirst(std::unique_ptr<Expression> first) { m_first = std::move(first); }
	void setSecond(std::unique_ptr<Expression> second) { m_second = std::move(second); }

protected:
	// `std::unique_ptr` automatically handles the lifetime of the child expressions
	std::unique_ptr<Expression> m_first;
	std::unique_ptr<Expression> m_second;
};

class Operator: virtual public Expression { };
class Function: virtual public Expression {
	int precedence() const override { return maxPrecedence; }
};

class UnaryOperator: public UnaryExpression, public Operator {
public:
	using UnaryExpression::UnaryExpression;

	virtual bool isPrefix() const = 0;

	void printInfixRecursive(std::ostream& output) const override final {
		const bool isPrefixOp = isPrefix();
		if (isPrefixOp) {
			printToken(output);
		}

		printInfixRecursiveOrPlaceholder(m_first.get(), output,
			m_first && m_first->precedence() < precedence());

		if (!isPrefixOp) {
			printToken(output);
		}
	}
};

class BinaryOperator: public BinaryExpression, public Operator {
	using BinaryExpression::BinaryExpression;

	void printInfixRecursive(std::ostream& output) const override final {
		printInfixRecursiveOrPlaceholder(m_first.get(), output,
			m_first && m_first->precedence() < precedence());

		output << ' ';
		printToken(output);
		output << ' ';

		printInfixRecursiveOrPlaceholder(m_second.get(), output,
			m_second && m_second->precedence() < precedence());
	}
};

class UnaryFunction: public UnaryExpression, public Function {
public:
	using UnaryExpression::UnaryExpression;

	void printInfixRecursive(std::ostream& output) const override final {
		printToken(output);

		output << '(';
		printInfixRecursiveOrPlaceholder(m_first.get(), output);
		output << ')';
	}
};

class BinaryFunction: public BinaryExpression, public Function {
public:
	using BinaryExpression::BinaryExpression;

	void printInfixRecursive(std::ostream& output) const override final {
		printToken(output);

		output << '(';
		printInfixRecursiveOrPlaceholder(m_first.get(), output);
		output << ", ";

		printInfixRecursiveOrPlaceholder(m_second.get(), output);
		output << ')';
	}
};

#endif
