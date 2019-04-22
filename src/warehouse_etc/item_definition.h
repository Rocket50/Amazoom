#ifndef AMAZOOM_WAREHOUSE_ETC_ITEM_DEFINITION_H_
#define AMAZOOM_WAREHOUSE_ETC_ITEM_DEFINITION_H_

#include "warehouse_etc/item_properties.h"

#include <iostream>

namespace amazoom {
	//this class represents a  sellable obj in the warehouse.
	class Item {

	public:
		Item(int itemID, float weight);
		Item(int itemID, float weight, bool isLarge);
		~Item();

		Item(Item &item) = delete; //copy constructor - transfer rhs item to lhs, set rhs to invalid
		Item& operator= (Item &item) = delete; //copy assignment - not allowed; items cannot be duplicated

											   //move assignment
		Item& operator= (Item &&item);

		//move constructor
		Item(Item&& item);

		int getID() const;
		float getWeight() const;
		bool isLarge() const;

		//this is member style overloading
		bool operator== (const Item &item); //overload item==item
											//checks item similarity by itemID only
		enum { INVALID_ITEM = -1 }; //when an item no longer exists because of move, destruction, etc

	private:

		amazoom::ItemProperties itemProp_;

		//Transfers internal "ItemProperties" from i2 to i1, and sets i2 properties as invalid enum
		//Returns i1
		amazoom::Item& swapAndClearProperties(Item& i1, Item& i2);
	};
	void printItem(const Item& i1);
	//this is normal overloading
	bool operator!= (const Item &i1, const Item &i2);

}

#endif 