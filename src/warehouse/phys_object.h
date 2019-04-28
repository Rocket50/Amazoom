#pragma once

#include "warehouse_etc/point.h"

namespace amazoom {
	//base class for defining an object in a warehouse
	class PhysObject {
	public:
		PhysObject();
		~PhysObject();

	protected:
		Point location;

	};
}