#ifndef AMAZOOM_CONTAINERS_HASHMAP_EXCEPTIONS_H
#define AMAZOOM_CONTAINERS_HASHMAP_EXCEPTIONS_H

#include <exception>
#include <string>

namespace amazoom {
class MultiHashMapNoSuchObj : public std::exception {

public:
	MultiHashMapNoSuchObj(std::string error) {};

	const char* what() const noexcept { return error_.c_str(); }

private:
	const std::string error_;
};
}


#endif
