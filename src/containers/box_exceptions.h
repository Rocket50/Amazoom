#ifndef AMAZOOM_CONTAINERS_BOX_EXCEPTIONS_H_
#define AMAZOOM_CONTAINERS_BOX_EXCEPTIONS_H_

#include <exception>
#include <string>

namespace amazoom {
class BoxOverweightException: public std::exception {

public:
	BoxOverweightException(std::string error) {};

	const char* what() const noexcept { return error_.c_str(); }

private:
	const std::string error_;
};
}

#endif