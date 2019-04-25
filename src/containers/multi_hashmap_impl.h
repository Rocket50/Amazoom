#ifndef AMAZOOM_CONTAINERS_MULTI_HASHMAP_IMPL_H_
#define AMAZOOM_CONTAINERS_MULTI_HASHMAP_IMPL_H_

#include "containers/worker_accessible_container.h"
#include "containers/multi_hashmap.h"

namespace amazoom { 

//Bridge to connect MultiHashMap to WorkerAccessibleContainer interface
class MultiHashmapImpl : public WorkerAccessibleContainer {
private:

	typedef amazoom::Item Item;
	typedef int Key;
	typedef std::function<bool(const Item& obj)> CompareFxn;

public:
	MultiHashmapImpl();
	~MultiHashmapImpl();

	virtual Item extractItem(const Key& key);
	virtual Item extractItem(const Key& key, const CompareFxn compareFxn);

	virtual void insertItem(Key key, Item& obj);

	virtual bool doesContainObj(const Key key);
	virtual bool doesContainObj(const Key key, const CompareFxn compareFxn);

	virtual int getNumItems();

private:

	MultiHashmap<Key, Item> storage_;

};
}

#endif

