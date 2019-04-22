#ifndef AMAZOOM_CONTAINERS_MULTI_HASHMAP_H
#define AMAZOOM_CONTAINERS_MULTI_HASHMAP_H

#include "warehouse_etc/item_definition.h"

#include "containers/multi_hashmap_exceptions.h"

#include <unordered_map>
#include <memory>
#include <functional>

namespace amazoom {
	/*Internally, storage is an unordered_map pointing to linkedlists. Reason: O(1) average case insertion and extraction
	* and O(N) worse case if all items have the same key, and a unique filter. sets are O(log(n)) worst, and average
	* unfortunately, due to the limitations of C+11 every hash map entry will point to a linked list
	* of other items who happen to have a similar hash
	* Because of the possibility of two different-key items producing the same hash, we cant pull front to back blindly
	* Secondly, pulling from the back of the linked list avoids having to rehash
	*/
	template <typename T_KEY, class T_OBJ>
	
	class MultiHashmap {

		class LinkedListNode; //forward declare
		class DataLinkedListNode;

		//Linked list class implementation, containing a key for referencing uniqueness
		//and data for data
	    typedef std::shared_ptr<LinkedListNode> NodePtr;
		typedef std::shared_ptr<DataLinkedListNode> DataNodePtr;
		typedef std::unordered_map<T_KEY, NodePtr> Map;


		//LinkedList default
		class LinkedListNode{
		public:
			LinkedListNode(NodePtr nxtptr) : nxtptr_(nxtptr) {}
			LinkedListNode() : nxtptr_(nullptr) {}
			NodePtr nxtptr_;
		};

		//LinkedList plus Data
		class DataLinkedListNode : public LinkedListNode {
		public:
			DataLinkedListNode(NodePtr nxtptr, T_KEY key, T_OBJ& obj) : LinkedListNode(nxtptr), key_(key), obj_(std::move(obj)) {}
			DataLinkedListNode(T_KEY key, T_OBJ& obj) : key_(key), obj_(std::move(obj)) {}
			T_KEY key_;
			T_OBJ obj_;
		};
		
	
	public:

		MultiHashmap();
		~MultiHashmap();

		int getNumItems() const; //returns how many items are currently stored

		/*Inserts an object into the container, indexed by a given key.
		* Throws MultiContainerFullException if the container is full.
		* If an exception is thrown, obj will not be modified. Successful insertion will
		* call obj's and key's move constructor
		*/
		void insertItem(T_KEY key, T_OBJ& obj);

		/*Allows the user to define a custom matching policy, and test the stored objects against it
		* Uses: When you have multiple objects with the same key, but different internal attributes
		*      you may narrow the search by giving this a custom matching policy function
		*/
		bool doesContainObj(const T_KEY& key, const std::function<bool(const T_OBJ& obj)> compareFxn) const;

		/*If no matching policy is provided, will search purely by key
		*/
		bool doesContainObj(const T_KEY& key) const;

		/*Attempts to extract any object matching key. If no such object can be found
		* MultiHashMapNoSuchObj is thrown
		*/
		T_OBJ extractItem(const T_KEY& key);

		/*Attempts to extract any object matching key that also returns true when put through a
		* user-defined custom matching policy. If no such item can be found, MultiHashMapNoSuchObj is thrown
		*/
		T_OBJ extractItem(const T_KEY& key, const std::function<bool(const T_OBJ& obj)> compareFxn);

	private:
		int currentNumItems; //how many items are stored
		const std::function<bool(const T_OBJ& obj)> defaultCompareFxn_;
		Map storInternal_;
	};
}

template <typename T_KEY, class T_OBJ>
inline amazoom::MultiHashmap<T_KEY, T_OBJ>::MultiHashmap(): storInternal_(Map()), 
															currentNumItems(0), 
															defaultCompareFxn_([](const T_OBJ&)->bool {return true; }) {
}

template <typename T_KEY, class T_OBJ>
inline amazoom::MultiHashmap<T_KEY, T_OBJ>::~MultiHashmap() {
}

template<typename T_KEY, class T_OBJ>
inline int amazoom::MultiHashmap<T_KEY, T_OBJ>::getNumItems() const{
	return currentNumItems;
}

//may throw MultiContainerFullException
template <typename T_KEY, class T_OBJ>
inline void amazoom::MultiHashmap<T_KEY, T_OBJ>::insertItem(T_KEY key, T_OBJ& obj) {
	if (storInternal_.count(key)) { //if the root node already exists
		//grab the root node
		NodePtr rootNodePtr(storInternal_.at(key));
		
		//All new nodes containing data are placed after the root node.
		//having a constant, no-data containing root node avoids the inefficiency of having to re-insert
		//the linkedlist into the hashmap every time we extract an object.
		//The topology of the hashmap is below
		//  
		//| HASH FXN |   |ROOT  NODE|   |DataLinkedListNode(currentNode)|    |  DataLinkedListNode(nextNode) |
		//|__________|   |__________|   |_______________________________|    |_______________________________|
		//|key-------|---|->nxtptr--|---|->nxtptr(initialize and point)-|----|--------> nxtptr(unchanged)----|-->
		//|          |   |(change to|   |         to the next node      |    |							     |
		//|          |   | new Node)|   |                               |    |                               |
		//------------   ------------   ---------------------------------    ---------------------------------
		NodePtr nextNodePtr(rootNodePtr->nxtptr_);

		//create newnode to be inserted into linked list, and connect it to the next node
		NodePtr newNode = std::make_shared<DataLinkedListNode>(nextNodePtr, key, obj);

		//re-link the first node
		rootNodePtr->nxtptr_ = newNode;
	}
	else { //there does not exist other items stored at this hash so create an empty root node, and linked it to the new node

		//create node the connects to the root. This contains key, and obj
		NodePtr dataNodePtr(std::make_shared<DataLinkedListNode>(nullptr, key, obj));

		//create empty root node and link it to our new data node
		auto newRootNode(std::make_shared<LinkedListNode>(dataNodePtr));

		//insert empty root node
		storInternal_.insert(std::make_pair<T_KEY, NodePtr>(std::move(key), std::move(newRootNode)));
	}

	currentNumItems++;
	
}

template<typename T_KEY, class T_OBJ>
inline bool amazoom::MultiHashmap<T_KEY, T_OBJ>::doesContainObj(const T_KEY& key, const std::function<bool(const T_OBJ& obj)> compareFxn) const {

	if (storInternal_.count(key) == 0) {
		return false;
	}
	else {
		//the root exists, now traverse the linked list and search for an object with a matching key
		DataNodePtr currentNodePtr = std::static_pointer_cast<DataLinkedListNode>(storInternal_.at(key)->nxtptr_);

		while (currentNodePtr != nullptr) {
			if (currentNodePtr->key_ == key && compareFxn(currentNodePtr->obj_)) {
				return true;
			}
			currentNodePtr = std::static_pointer_cast<DataLinkedListNode>(currentNodePtr->nxtptr_);
		}
		return false;
	}

}


template<typename T_KEY, class T_OBJ>
inline bool amazoom::MultiHashmap<T_KEY, T_OBJ>::doesContainObj(const T_KEY& key) const {
	return doesContainObj(key, defaultCompareFxn_);
}

template <typename T_KEY, class T_OBJ>
inline T_OBJ amazoom::MultiHashmap<T_KEY, T_OBJ>::extractItem(const T_KEY& key){

	//no special filtering policy, so return true
	return extractItem(key, defaultCompareFxn_);
}

template<typename T_KEY, class T_OBJ>
inline T_OBJ amazoom::MultiHashmap<T_KEY, T_OBJ>::extractItem(
	const T_KEY& key, const std::function<bool(const T_OBJ&obj)> compareFxn) {


	//if no items exist with matching key

	std::cout << storInternal_.count(key);
	if (!storInternal_.count(key)) {
		throw MultiHashMapNoSuchObj("Container does not contain object matching key: " + key);
	}
	//first item is always the root node. Nodes that contain actual data begin at the second node
	NodePtr rootPtr = storInternal_.at(key);
	NodePtr currentNodePtr = rootPtr->nxtptr_;
	NodePtr prevNode(rootPtr);

	while (currentNodePtr->nxtptr_ != nullptr) {
		if (currentNodePtr == nullptr) {
			throw MultiHashMapNoSuchObj("Container does not contain object matching special params and key: " + key);
		}

		auto castedNodePtr = std::static_pointer_cast<DataLinkedListNode>(currentNodePtr);

		//key must match, and user defined matching policy must be true
		if (castedNodePtr->key_ == key && compareFxn(castedNodePtr->obj_)) { break; };

		prevNode = currentNodePtr;
		currentNodePtr = currentNodePtr->nxtptr_;

		//if we are at the end of the linkedlist and no item matching these paramters can be found
		
	}
	// extract the contents of the item
	T_OBJ extractedObj(std::move(std::static_pointer_cast<DataLinkedListNode>(currentNodePtr)->obj_));

	//detach this node from the linked list
	prevNode->nxtptr_ = currentNodePtr->nxtptr_;

	currentNumItems--;
	return std::move(extractedObj);
}

#endif
