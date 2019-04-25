#ifndef AMAZOOM_CONTAINERS_ITEM_CONTAINER_EXCEPTIONS_H_
#define AMAZOOM_CONTAINERS_ITEM_CONTAINER_EXCEPTIONS_H_

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

#endif