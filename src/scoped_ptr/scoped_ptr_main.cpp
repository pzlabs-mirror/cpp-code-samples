#include "scoped_ptr/scoped_ptr.hpp"

#include <string>
#include <iostream>

/// Use case 1.
/// A polymorphic object which is managed through a pointer to the the base class, and therefore
/// has to be allocated dynamically
class ILogger {
public:
	virtual ~ILogger() = default;
	virtual void write(const std::string& message) = 0;
};

class ConsoleLogger: public ILogger {
public:
	virtual void write(const std::string& message) {
		std::cout << message << '\n';
	}
};

/// Use case 2.
/// A very large object that potentionally overflows the stack
struct Config {
	double data[1'000'000]{};
};

class Manager {
public:
	Manager(ScopedPtr<ILogger> logger, Config* config):
		m_logger(std::move(logger)),
		m_config(config)
	{ }

	void process() {
		m_logger->write("First: " + std::to_string(m_config->data[0]));
	}

private:
	ScopedPtr<ILogger> m_logger;
	ScopedPtr<Config> m_config;
};

int main() {
	Manager manager(ScopedPtr<ConsoleLogger>(new ConsoleLogger()), new Config{2.3, 4.5});
	manager.process();
	// Automatic cleanup of `ConsoleLogger` and `Config` when `manager` goes out of scope
}
