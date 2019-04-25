#include "box.h"


amazoom::Box::Box(const float max_weight) : MAX_WEIGHT_(max_weight){}

amazoom::Box::Box(StoragePtr& storage,
	              const float max_weight) : ItemContainer(storage), MAX_WEIGHT_(max_weight) {}

amazoom::Box::~Box() { }

bool amazoom::Box::canInsert(const amazoom::Item& item) {
	boost::shared_lock<boost::shared_mutex> lock(mtx_);
	return !wouldBeOverweight(item);
}

amazoom::Item amazoom::Box::extractItem(int key) {
	boost::unique_lock<boost::shared_mutex> lock(mtx_);

	amazoom::Item extractedItem(storage_->extractItem(key));
	currWeight_ -= extractedItem.getWeight();

	return std::move(extractedItem);
}

void amazoom::Box::insertItem(Item& item) {
	boost::unique_lock<boost::shared_mutex> lock(mtx_);
	if (wouldBeOverweight(item)) {
		throw BoxOverweightException("Item too heavy to be inserted.");  
	}
	
	currWeight_ += item.getWeight();
	storage_->insertItem(item.getID(),item);
}

float amazoom::Box::currentWeight() {
	boost::unique_lock<boost::shared_mutex> lock(mtx_);
	return currWeight_;
}

float amazoom::Box::getMaxWeight() {
	boost::shared_lock<boost::shared_mutex> lock(mtx_);
	return MAX_WEIGHT_;
}

bool amazoom::Box::wouldBeOverweight(const Item& item)  {
	//do not mutex, calling functions should be protected
	return item.getWeight() + currWeight_ > MAX_WEIGHT_;
}

