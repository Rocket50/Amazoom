#pragma once

#include "warehouse/phys_object.h"

namespace amazoom {
//Object that can move or whose state can change
class DynamicObject : public PhysObject{
public:
	DynamicObject();
	virtual ~DynamicObject();
};
}