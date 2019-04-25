#ifndef AMAZOOM_CONTAINERS_MULTI_HASHMAP_H_
#define AMAZOOM_CONTAINERS_MULTI_HASHMAP_H_

#include "warehouse_etc/item_definition.h"

#include <unordered_map>
#include <memory>
#include <functional>

#include "containers/multi_hashmap_exceptions.h"
#include "boost/thread.hpp"

namespace amazoom {
	/* A multihashmap allows for quick insertions of objects, mapped by keys that need not be unique.
	*  Internally, storage is an unordered_map pointing to linkedlists. O(1) average case insertion 
	*  and extraction, and O(N) worse case if all items have the same key, and chooses to select 
	*  by a unique filter. Performs closer to O(1) if there are are many keys
	*  Thread-safe. Allows multiple simutaneous reads, and single extraction/insertions
	*/
	template <typename T_KEY, class T_OBJ>
	class MultiHashmap {

		class LinkedListNode; //forward declare
		class DataLinkedListNode;

		//Linked list implementation
	    typedef std::shared_ptr<LinkedListNode> NodePtr;
		typedef std::shared_ptr<DataLinkedListNode> DataNodePtr;
		typedef std::unordered_map<T_KEY, NodePtr> Map;
		typedef std::function<bool(const Item& obj)> CompareFxn;

		//LinkedList default. Only used for the first node of all linked lists
		class LinkedListNode {
		public:
			LinkedListNode(NodePtr nxtptr = nullptr) : nxtptr_(nxtptr) {}
			NodePtr nxtptr_{};
		};

		//LinkedList plus Data. Used for all nodes after the root node.
		//Having an unmovable root node avoids the problem of having to
		//reinsert nodes into the unordered_map should the first node
		//be extracted.
		class DataLinkedListNode : public LinkedListNode {
		public:
			DataLinkedListNode(NodePtr nxtptr, T_KEY key, T_OBJ& obj) 
				: LinkedListNode(nxtptr), key_(key), obj_(std::move(obj)) {}

			DataLinkedListNode(T_KEY key, T_OBJ& obj) : key_(key), obj_(std::move(obj)) {}

			T_KEY key_;
			T_OBJ obj_;
		};
		
	public:
		MultiHashmap();

		MultiHashmap(const MultiHashmap<T_KEY, T_OBJ>& hashmap) = delete; //copy constructor to-do
		MultiHashmap<T_KEY, T_OBJ>& operator=(const MultiHashmap<T_KEY, T_OBJ>& hashmap) = delete; //assignment to-do

		~MultiHashmap();

		int getNumItems() const; //returns how many items are currently stored

		//Inserts an object into the container indexed by a key.
		void insertItem(T_KEY key, T_OBJ& obj);

		/*Checks whether the class contains an object stored using this key, that also satisfies a user-defined
		* comparison function, compareFxn. 
		*
		* Uses: When there are multiple objects stored with the same key, you may narrow the search 
		* by giving this a custom comparison function.
		*
		* Example of calling this function: 
		* int key = 0;
		* thisHashmap.doesContainObj(key, [desiredMemberGet, desiredAttribute](const T_OBJ& obj)->bool {
		*  return (obj.getSomeMember() == desiredMemberGet && obj.attribute >= desiredAttribute; }}; 
		*/
		bool doesContainObj(const T_KEY& key, const CompareFxn compareFxn) const;

		/*If no comparison function is provided, will search purely by key */
		bool doesContainObj(const T_KEY& key) const;

		/*Extracts any object matching key. If no such object can be found MultiHashMapNoSuchObj is thrown*/
		T_OBJ extractItem(const T_KEY& key);

		/*Extracts the first time found that matches this key that also satifies a 
		* user-defined comparison function, compareFxn.
		* If no object matching these parms are found, MultiHashMapNoSuchObj is thrown.
		*
		* Example of calling this function:
		* int key = 0;
		* thisHashmap.extractItem(key, [desiredMemberGet, desiredAttribute](const T_OBJ& obj)->bool {
		*  return (obj.getSomeMember() == desiredMemberGet && obj.attribute < desiredAttribute; }};
		*/
		T_OBJ extractItem(const T_KEY& key, const CompareFxn compareFxn);

	private:
		int currentNumItems{ 0 }; //how many items are stored
		const CompareFxn defaultCompareFxn_{ 
			[](const T_OBJ&)->bool { return true; } 
		}; //returns true

		Map storInternal_;

		//class level mutex. Separates reading and writing operations
		mutable boost::shared_mutex mtx_;

	};
}

template <typename T_KEY, class T_OBJ>
inline amazoom::MultiHashmap<T_KEY, T_OBJ>::MultiHashmap(){}

template <typename T_KEY, class T_OBJ>
inline amazoom::MultiHashmap<T_KEY, T_OBJ>::~MultiHashmap() {
}

template<typename T_KEY, class T_OBJ>
inline int amazoom::MultiHashmap<T_KEY, T_OBJ>::getNumItems() const{
	boost::unique_lock<boost::shared_mutex> lock(mtx_);
	return currentNumItems;
}

template <typename T_KEY, class T_OBJ>
inline void amazoom::MultiHashmap<T_KEY, T_OBJ>::insertItem(T_KEY key, T_OBJ& obj) {

	boost::unique_lock<boost::shared_mutex> lock(mtx_);
	

	if (storInternal_.count(key)) { //if the root node already exists
		//grab the root node
		NodePtr rootNodePtr(storInternal_.at(key));
		/*All new nodes are inserted directly after the root node.
		//The topology of the system is below
		//  
		//| HASH FXN |   |ROOT  NODE|   |DataLinkedListNode(currentNode)|    |  DataLinkedListNode(nextNode) |
		//|__________|   |__________|   |_______________________________|    |_______________________________|
		//|key-------|---|->nxtptr--|---|->nxtptr(initialize and point)-|----|--------> nxtptr(unchanged)----|-->
		//|          |   |(change to|   |         to the next node      |    |							     |
		//|          |   | new Node)|   |                               |    |                               |
		//------------   ------------   ---------------------------------    ---------------------------------
		*/

		NodePtr nextNodePtr(rootNodePtr->nxtptr_);

		//create newnode to be inserted into linked list, and connect to the node that was originally in its place
		NodePtr newNode = std::make_shared<DataLinkedListNode>(nextNodePtr, key, obj);

		//re-link the first node
		rootNodePtr->nxtptr_ = newNode;
	}
	else { //There are no items stored at this hash. Create an empty root node, a data node and connect them.

		//create node the connects to the root. This contains key, and obj
		NodePtr dataNodePtr(std::make_shared<DataLinkedListNode>(nullptr, key, obj));

		//create empty root node and link it to our new data node
		auto newRootNode(std::make_shared<LinkedListNode>(dataNodePtr));

		//insert empty root node
		storInternal_.insert(std::make_pair<T_KEY, NodePtr>(std::move(key), std::move(newRootNode)));
	}

	//lock counter
	currentNumItems++;
	
}

template<typename T_KEY, class T_OBJ>
inline bool amazoom::MultiHashmap<T_KEY, T_OBJ>::doesContainObj(const T_KEY& key, const CompareFxn compareFxn) const {
	boost::unique_lock<boost::shared_mutex> lock(mtx_);

	if (storInternal_.count(key) == 0) {
		return false;
	}
	else {
		//the root exists, now traverse the linked list and search for an object with a matching key that satisfies the compare function
		DataNodePtr currentNodePtr = std::static_pointer_cast<DataLinkedListNode>(storInternal_.at(key)->nxtptr_);

		while (currentNodePtr != nullptr) {
			if (currentNodePtr->key_ == key && compareFxn(currentNodePtr->obj_)) {
				return true;
			}
			currentNodePtr = std::static_pointer_cast<DataLinkedListNode>(currentNodePtr->nxtptr_);
		}
	}
	return false;
}


template<typename T_KEY, class T_OBJ>
inline bool amazoom::MultiHashmap<T_KEY, T_OBJ>::doesContainObj(const T_KEY& key) const {
	//mutex inside
	return doesContainObj(key, defaultCompareFxn_);
}

template <typename T_KEY, class T_OBJ>
inline T_OBJ amazoom::MultiHashmap<T_KEY, T_OBJ>::extractItem(const T_KEY& key){
	//mutex inside
	return extractItem(key, defaultCompareFxn_);
}

template<typename T_KEY, class T_OBJ>
inline T_OBJ amazoom::MultiHashmap<T_KEY, T_OBJ>::extractItem(
	const T_KEY& key, const CompareFxn compareFxn) {

	boost::unique_lock<boost::shared_mutex> lock(mtx_);

	if (!storInternal_.count(key)) {
		throw MultiHashMapNoSuchObj("Container does not contain object matching key: " + key);
	}
	
	//first item is always the root node. Nodes that contain actual data begin after
	NodePtr rootPtr = storInternal_.at(key); 
	NodePtr currentNodePtr = rootPtr->nxtptr_;
	NodePtr prevNode(rootPtr);

	//only root exists, no data node. this occurs when all items matching this key have been taken out
	if (rootPtr->nxtptr_ == nullptr) {
		throw MultiHashMapNoSuchObj("Container does not contain object matching key: " + key);
	}

	while (currentNodePtr->nxtptr_ != nullptr) {
		if (currentNodePtr == nullptr) {
			//reached the end of the linked list, no matches
			throw MultiHashMapNoSuchObj("Container does not contain object matching special params and key: " + key);
		}

		auto castedNodePtr = std::static_pointer_cast<DataLinkedListNode>(currentNodePtr);

		//key must match, and user defined comparison function must return true
		if (castedNodePtr->key_ == key && compareFxn(castedNodePtr->obj_)) { break; };

		prevNode = currentNodePtr;
		currentNodePtr = currentNodePtr->nxtptr_;
	}
	// extract the data content of the node
	T_OBJ extractedObj(std::move(std::static_pointer_cast<DataLinkedListNode>(currentNodePtr)->obj_));

	//detach this node from the linked list
	prevNode->nxtptr_ = currentNodePtr->nxtptr_;

	currentNumItems--;

	return std::move(extractedObj);
}

#endif
