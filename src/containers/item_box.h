#ifndef AMAZOOM_CONTAINERS_ITEM_BOX_H_
#define AMAZOOM_CONTAINERS_ITEM_BOX_H_

#include "containers/item_container.h"

#include "containers/item_box_exceptions.h"

#include "boost/thread.hpp"

namespace amazoom {

/* Class representation of a box. Limits insertions due to a maximum weight
*
*/
class ItemBox : public ItemContainer {
public:
	ItemBox(float max_weight = 100.0f);
	~ItemBox();

	virtual bool canInsert(const amazoom::Item& item) override;
	virtual amazoom::Item extractItem(int key) override;
	virtual void insertItem(Item& item) override;
	virtual float currentWeight();

protected:

private:
	//checks if adding the item would make the object overweight
	bool wouldBeOverweight(const Item& item);

	const float MAX_WEIGHT_;
	float currWeight_{ 0.0f };

	boost::shared_mutex mtx_;
};
}

#endif