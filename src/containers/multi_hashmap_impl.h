#ifndef AMAZOOM_CONTAINERS_MULTI_HASHMAP_IMPL_H_
#define AMAZOOM_CONTAINERS_MULTI_HASHMAP_IMPL_H_

#include "containers/item_storage.h"
#include "containers/multi_hashmap.h"

namespace amazoom { 

	//Bridge to connect MultiHashMap to ItemStorage interface
	class MultiHashmapImpl : public ItemStorage {
	private:

		typedef amazoom::Item Item;
		typedef int Key;


	public:
		MultiHashmapImpl();
		~MultiHashmapImpl();

		virtual Item extractItem(const Key& key);
		virtual Item extractItem(const Key& key, const std::function<bool(const Item& obj)> compareFxn);

		virtual void insertItem(Key key, Item& obj);

		virtual bool doesContainObj(const Key key);
		virtual bool doesContainObj(const Key key, const std::function<bool(const Item& obj)> compareFxn);

		virtual int getNumItems();

	private:

		MultiHashmap<Key, Item> storage_;

	};
}

#endif

