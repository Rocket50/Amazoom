#include "multi_hashmap_impl.h"

amazoom::MultiHashmapImpl::MultiHashmapImpl() {}

amazoom::MultiHashmapImpl::~MultiHashmapImpl() {}

amazoom::Item amazoom::MultiHashmapImpl::extractItem(const Key & key) {
	return std::move(storage_.extractItem(key));
}

amazoom::Item amazoom::MultiHashmapImpl::extractItem(const Key & key, CompareFxn compareFxn) {
	return std::move(storage_.extractItem(key, compareFxn));
}

void amazoom::MultiHashmapImpl::insertItem(Key key, Item & obj) {
	storage_.insertItem(key, obj);
}

bool amazoom::MultiHashmapImpl::doesContainObj(const Key key) {
	return storage_.doesContainObj(key);
}

bool amazoom::MultiHashmapImpl::doesContainObj(const Key key, CompareFxn compareFxn) {
	return storage_.doesContainObj(key, compareFxn);
}

int amazoom::MultiHashmapImpl::getNumItems() {
	return storage_.getNumItems();
}
