#include <iostream>
#include <iomanip>

#include "bitset/bitset.hpp"

struct LineSeparator {
	int length = 40;
	char symbol = '-';

	friend std::ostream& operator<<(std::ostream& out, const LineSeparator& line) {
		out << std::setfill(line.symbol) << std::setw(line.length + 1) << '\n';
		return out;
	}
};

int main() {
	BitSet a;
	std::cout << "Enter set a: ";
	std::cin >> a;

	if (std::cin.bad()) {
		std::cerr << "Fatal error\n";
		return 1;
	}

	BitSet b;
	b.set(2);
	b[3] = true;
	b[9] = true;

	{
		std::cout << '\n' << LineSeparator{40, '='};
		std::cout << "a -> " << a << '\n';
		std::cout << "b -> " << b << '\n';
	}

	const LineSeparator line{40};
	std::cout << std::boolalpha;

	{
		std::cout << line << "Testing BitSet::get and BitSet::operator[]:\n\n";

		struct SetRef { const char* name; BitSet& ref; };
		for (SetRef setRef: {SetRef{"a", a}, SetRef{"b", b}}) {
			using SizeArr = size_t[];
			for (size_t num : SizeArr{0, 1, 2, 3, 5, 7, 8, 9, 15, 45, 120}) {
				std::cout << setRef.name << ".get(" << num << ") -> " << setRef.ref.get(num) << "; ";
				std::cout << setRef.name << "[" << num << "] -> " << bool{setRef.ref[num]} << '\n';
			}
		}
	}

	{
		std::cout << line << "Testing set operations: \n\n";

		std::cout << "a | b -> " << (a | b) << '\n';
		std::cout << "a & b -> " << (a & b) << '\n';
		std::cout << "a ^ b -> " << (a ^ b) << '\n';
	}

	{
		std::cout << line << "Testing comparison operators: \n\n";

		std::cout << "a == b -> " << (a == b) << '\n';
		std::cout << "a != b -> " << (a != b) << '\n';
		std::cout << "a == a -> " << (a == a) << '\n';
		std::cout << "a != a -> " << (a != a) << '\n';
		std::cout << "b == b -> " << (b == b) << '\n';
		std::cout << "b != b -> " << (b != b) << '\n';
	}

	{
		std::cout << line << "Testing copy constructor and copy assignment: \n\n";

		BitSet c(a);
		std::cout << "BitSet c(a) -> " << c << '\n';
		c = b;
		std::cout << "(c = b) -> " << c << '\n';
	}

	{
		std::cout << line << "Testing BitSet swap: \n\n";

		swap(a, b);
		std::cout << "a -> " << a << '\n';
		std::cout << "b -> " << b << '\n';
	}

	{
		std::cout << line << "Testing BitSet::clear: \n\n";

		a.clear();
		b.clear();
		std::cout << "a -> " << a << '\n';
		std::cout << "b -> " << b << '\n';
	}
}
