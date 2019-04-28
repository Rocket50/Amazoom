#pragma once
#include "notifiable.h"

#include <vector>
#include <boost/thread.hpp>

namespace amazoom {
	//class representing the physical representation of the warehouse

	class Notifiable;

class Warehouse {
public:
	
	typedef std::shared_ptr<amazoom::Notifiable> Observer;

	Warehouse();
	~Warehouse();
	void attach(Observer obsvr);
	void detach(Observer obsvr);

	void getState();

private:

	std::vector<Observer> obsvrs_;
	boost::mutex mtx_;

};
}