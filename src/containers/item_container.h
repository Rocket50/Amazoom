#ifndef AMAZOOM_CONTAINERS_ITEM_CONTAINER_H_
#define AMAZOOM_CONTAINERS_ITEM_CONTAINER_H_



#include "warehouse_etc/item_definition.h"
#include "containers/multi_hashmap_impl.h"
#include "containers/worker_accessible_container.h"

#include <boost/thread.hpp>
#include <memory>

namespace amazoom {
	//ItemContainer is an interface from which other more specialized containers may be built
	class ItemContainer {
	private:
		typedef std::unique_ptr<WorkerAccessibleContainer> StoragePtr;

	public:
		ItemContainer();

		//Dependency Injection
		ItemContainer(StoragePtr& storage);
		~ItemContainer();

		//Overrideable function that allows a derived class to 
		//set their own insertion policy. Returns true by default.
		//thread-safe by default.
		virtual bool canInsert(const amazoom::Item& item);

		//Overrideable function that allows a derived class to 
		//set their own extraction policy. Returns true if
		//the container contains a object matching this key.
		//thread-safe by default.
		virtual bool canExtract(int key);

		bool doesContainItem(int key);

		//User implemented extraction function
		virtual amazoom::Item extractItem(int key) = 0;

		//User implemented insertion function
		virtual void insertItem(Item& item) = 0;

	protected:
		StoragePtr storage_{ std::unique_ptr<MultiHashmapImpl>(new MultiHashmapImpl()) };
	};

}

#endif