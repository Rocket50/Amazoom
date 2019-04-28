#pragma once

#include "warehouse.h"

#include <memory>

namespace amazoom {
	class Warehouse;

	//classes who must be notified if any state changes occur in the warehouse
	class Notifiable {
	public:
		Notifiable();
		~Notifiable();
		virtual void notify(std::shared_ptr<amazoom::Warehouse> wh) = 0;
	};

}