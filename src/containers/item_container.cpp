#include "item_container.h"

amazoom::ItemContainer::ItemContainer() {
}

amazoom::ItemContainer::ItemContainer(StoragePtr& storage) : storage_(std::move(storage)){}

amazoom::ItemContainer::~ItemContainer() {}

bool amazoom::ItemContainer::canInsert(const amazoom::Item& item) {
	return true;
}

bool amazoom::ItemContainer::canExtract(int key) {
	return storage_->doesContainObj(key);
}

bool amazoom::ItemContainer::doesContainItem(int key) {
	return storage_->doesContainObj(key);
}

