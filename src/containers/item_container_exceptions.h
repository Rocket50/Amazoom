#pragma once

#include <exception>
#include <string>

namespace amazoom {

class ItemContainerFullException : public std::exception {

public:
	ItemContainerFullException(std::string error) {};

	const char* what() const noexcept { return error_.c_str(); }

private:
	const std::string error_;
};


};
