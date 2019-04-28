#ifndef AMAZOOM_CONTAINERS_BOX_H_
#define AMAZOOM_CONTAINERS_BOX_H_

#include "containers/item_container.h"

#include "containers/box_exceptions.h"

#include "boost/thread.hpp"

namespace amazoom {

/* Class representation of a box. Limits insertions due to a maximum weight
*
*/
class Box : public ItemContainer {
private:
	typedef std::unique_ptr<WorkerAccessibleContainer> StoragePtr;


public:

	Box(const float max_weight = 100.0f); //uses the default scheme defined in ItemContainer
	Box(StoragePtr& storage, const float max_weight = 100.0f); //accepts custom key-value mapping schemes
	virtual ~Box(); 

	

	virtual bool canInsert(const amazoom::Item& item) override;
	virtual amazoom::Item extractItem(int key) override;
	virtual void insertItem(Item& item) override;
	virtual float currentWeight();
	virtual float getMaxWeight();

private:
	//checks if adding the item would make the object overweight
	bool wouldBeOverweight(const Item& item);

	const float MAX_WEIGHT_;
	float currWeight_{ 0.0f };

	boost::shared_mutex mtx_;
};
}

#endif