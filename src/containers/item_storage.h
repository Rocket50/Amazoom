#pragma once

#include "warehouse_etc/item_definition.h"

#include <functional>

namespace amazoom {
	/*Interfaces for swapping out an underlying storage class using Inversion of Control
	*
	*/
	class Checkable {
	private:
		typedef amazoom::Item Item;
		typedef int Key;
		

	public:
		//searches through the underlying storage scheme for an object purely by key
		virtual bool doesContainObj(const Key key) = 0;

		/*Searches through the underlying storage scheme for an object purely by key plus provides an optional
		* custom compare function to narrow the search.
		*/
		virtual bool doesContainObj(const Key key, const std::function<bool(const Item& obj)> compareFxn) = 0;

		//Return number of items currently stored
		virtual int getNumItems() = 0;
	};

	class Storable {
	private:
		typedef amazoom::Item Item;
		typedef int Key;
		


	public:
		//Searches through the underlying storage scheme for an object purely by key and extracts it
		virtual Item extractItem(const Key& key) = 0;

		/*Searches through the underlying storage scheme for an object purely by key plus provides an optional
		* custom compare function to narrow the search, and extracts it.
		*/
		virtual Item extractItem(const Key& key, const std::function<bool(const Item& obj)> compareFxn) = 0;

		/*Inserts an item obj, and indexes it by key
		* custom compare function to narrow the search, and extracts it.
		*/
		virtual void insertItem(Key key, Item& obj) = 0;
	};

	class ItemStorage : public Storable, public Checkable{ };
}