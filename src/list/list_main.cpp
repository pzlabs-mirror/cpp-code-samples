#include "list/list.hpp"

#include <list>
#include <iostream>
#include <string_view>

template<typename T>
static void print(std::string_view prefix, const List<T>& list) {
	std::cout << prefix;
	bool first = true;
	std::cout << '{';
	for (const auto& e : list) {
		if (!first)
			std::cout << ", ";
		std::cout << e;
		first = false;
	}
	std::cout << "} (size: " << list.size() << ")\n" << std::flush;
}

int main() {
	std::cout << std::boolalpha;

	std::cout << "Create an empty list<int>:\n";
	List<int> listA;
	print("A: ", listA);

	std::cout << "Push 4 values:\n";
	listA.push_back(1);
	listA.push_back(2);
	listA.push_back(3);
	listA.push_back(4);
	print("A: ", listA);

	std::cout << "Insert a value at the end:\n";
	listA.insert(listA.end(), 5);
	print("A: ", listA);

	std::cout << "Insert a value at the start:\n";
	listA.insert(listA.begin(), -1);
	print("A: ", listA);

	std::cout << "Insert a value in the middle:\n";
	listA.insert(std::next(listA.begin(), 2), -2);
	print("A: ", listA);

	std::cout << "Erase a value:\n";
	auto after = listA.erase(std::next(listA.begin(), 3));
	print("A: ", listA);
	std::cout << "Erased before: " << *after << '\n';

	std::cout << "Copy list:\n";
	auto listB = listA;
	print("B: ", listB);
	print("A: ", listA);

	std::cout << "Compare the copy and the original:\n";
	std::cout << "Equal: " << (listB == listA) << ". Not equal: " << (listB != listA) << '\n';
}
