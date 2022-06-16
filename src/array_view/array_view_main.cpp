#include <vector>
#include <iostream>

#include "array_view/array_view.hpp"

static void printArray(ArrayView<const double> arr) {
	std::cout << '{';
	for (auto it = arr.begin(); it != arr.end(); ++it) {
		if (it != arr.begin()) {
			std::cout << ", ";
		}
		std::cout << *it;
	}
	std::cout << '}';
}

static void multiplyEachBy(ArrayView<double> arr, double coef) {
	for (double& elem : arr) {
		elem *= coef;
	}
}

static double sum(ArrayView<const double> arr) {
	double result = 0.;
	for (double value : arr) {
		result += value;
	}
	return result;
}

static double product(ArrayView<const double> arr) {
	double result = 1.;
	for (size_t i = 0; i < arr.size(); ++i) {
		result *= arr[i];
	}
	return result;
}

int main() {
	{
		// ArrayView is compatible with C arrays
		double numbers[] = {6, 2, 3, 5};

		std::cout << "Array: ";
		printArray(numbers);

		std::cout << "\nFirst half: ";
		printArray(ArrayView<double>(numbers).subview(0, std::size(numbers) / 2u));

		std::cout << "\nSum: " << sum(numbers);

		ArrayView<const double> view(numbers, numbers + std::size(numbers));
		std::cout << "\nProduct: " << product(view);

		std::cout << "\nAfter doubling: ";
		multiplyEachBy(numbers, 2.);
		printArray(numbers);
		std::cout << '\n';
	}

	{
		// ArrayView is compatible with contiguous containers
		std::vector<double> vec = {6, 2, 3, 5};

		std::cout << "\nVector: ";
		printArray({vec.begin(), vec.end()});

		std::cout << "\nSecond half: ";
		printArray(ArrayView<double>(vec.begin(), vec.size())
			.subview(vec.size() / 2));

		ArrayView<const double> view(vec.begin(), vec.end());
		std::cout << "\nProduct: " << product(view);

		std::cout << '\n';
	}
}
