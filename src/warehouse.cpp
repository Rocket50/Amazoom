#include "warehouse.h"

amazoom::Warehouse::Warehouse() {
}

amazoom::Warehouse::~Warehouse() {
}

void amazoom::Warehouse::attach(Observer obsvr) {
	if (obsvr == nullptr) {
		throw std::invalid_argument("Attempted to add a null observer as a listener!");
	}
	boost::lock_guard<boost::mutex> lock(mtx_);
	obsvrs_.push_back(obsvr);
}

void amazoom::Warehouse::detach(Observer obsvr) {
	boost::lock_guard<boost::mutex> lock(mtx_);
}

void amazoom::Warehouse::getState() {
	boost::lock_guard<boost::mutex> lock(mtx_);
}
