#ifndef AMAZOOM_CONTAINERS_ITEM_BOX_EXCEPTIONS_H_
#define AMAZOOM_CONTAINERS_ITEM_BOX_EXCEPTIONS_H_

#include <exception>
#include <string>

namespace amazoom {
class ItemBoxOverweightException: public std::exception {

public:
	ItemBoxOverweightException(std::string error) {};

	const char* what() const noexcept { return error_.c_str(); }

private:
	const std::string error_;
};
}

#endif