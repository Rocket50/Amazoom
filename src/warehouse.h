#pragma once

#include "warehouse_model.h"
#include "notifiable.h"
#include "warehouse/dynamic_object.h"

#include <vector>
#include <boost/thread.hpp>

namespace amazoom {
class Notifiable; //fwd declare

//class for interacting with the warehouise
class Warehouse {
public:
	static enum Cardinal {NORTH, SOUTH, EAST, WEST};
	typedef std::shared_ptr<Notifiable> Observer;

	Warehouse();
	~Warehouse();
	void attach(Observer obsvr);
	void detach(Observer obsvr);
	
	//moving an object to one tile in a certain direction
	void move(std::shared_ptr<DynamicObject> objToMove, Cardinal dir);

	//move an object to a different position
	void move(std::shared_ptr<DynamicObject> objToMove, Point p);

	void getState();

private:

	std::vector<Observer> obsvrs_;
	boost::mutex mtx_;
	WarehouseModel model_;

};
}