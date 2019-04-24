#include "containers/item_box.h"

amazoom::ItemBox::ItemBox(float max_weight) : MAX_WEIGHT_(max_weight){}

amazoom::ItemBox::~ItemBox() { }

bool amazoom::ItemBox::canInsert(const amazoom::Item& item) {
	boost::unique_lock<boost::shared_mutex> lock(mtx_);
	return !wouldBeOverweight(item);
}

amazoom::Item amazoom::ItemBox::extractItem(int key) {
	boost::unique_lock<boost::shared_mutex> lock(mtx_);

	amazoom::Item extractedItem(storage_->extractItem(key));
	currWeight_ -= extractedItem.getWeight();

	return std::move(extractedItem);
}

void amazoom::ItemBox::insertItem(Item& item) {
	boost::unique_lock<boost::shared_mutex> lock(mtx_);
	if (wouldBeOverweight(item)) {
		throw ItemBoxOverweightException("Item too heavy to be inserted.");  
	}
	
	currWeight_ += item.getWeight();
	storage_->insertItem(item.getID(),item);
}

float amazoom::ItemBox::currentWeight() {
	boost::unique_lock<boost::shared_mutex> lock(mtx_);
	return currWeight_;
}

bool amazoom::ItemBox::wouldBeOverweight(const Item& item) {
	//do not mutex, calling functions should be protected
	return item.getWeight() + currWeight_ >= MAX_WEIGHT_;
}

