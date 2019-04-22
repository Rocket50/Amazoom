
#include "warehouse_etc/item_definition.h"

amazoom::Item::Item(int itemID, float weight)
	: itemProp_{ itemID, weight, false } {}

amazoom::Item::Item(int itemID, float weight, bool isLarge)
	: itemProp_{ itemID, weight, isLarge } {}


amazoom::Item::~Item() {}

/*amazoom::Item::Item(Item& item) {
swapAndClearProperties(*this, item);
}*/

/*Amazoom::Item& Amazoom::Item::operator= (Item &item) {
return swapAndClearProperties(*this, item);
}*/

amazoom::Item& amazoom::Item::operator= (Item&& item) {
	return swapAndClearProperties(*this, item);
}

void amazoom::printItem(const amazoom::Item& i1) {
	std::cout << "This item has an id: " << i1.getID() << ", is weight: " << i1.getWeight() << '\n';
}

bool amazoom::operator!=(const Item & i1, const Item & i2) {
	return(i1.getID() != i2.getID());
}

amazoom::Item::Item(Item && item)
	: itemProp_{ item.itemProp_.itemID, item.itemProp_.weight, item.itemProp_.isLarge } {

	//i2 shouldn't be used anymore, so set to invalid
	item.itemProp_.itemID = INVALID_ITEM;
}

int amazoom::Item::getID() const {
	return itemProp_.itemID;
}

float amazoom::Item::getWeight() const {
	return itemProp_.weight;
}

bool amazoom::Item::isLarge() const {
	return itemProp_.isLarge;
}

bool amazoom::Item::operator== (const amazoom::Item& item) {
	return (itemProp_.itemID == item.itemProp_.itemID);
}

amazoom::Item& amazoom::Item::swapAndClearProperties(Item & i1, Item & i2) {
	//transfer state i2 to i1
	i1.itemProp_.weight = i2.itemProp_.weight;
	i1.itemProp_.itemID = i2.itemProp_.itemID;
	i1.itemProp_.isLarge = i2.itemProp_.isLarge;

	//i2 shouldn't be used anymore, so set to invalid
	i2.itemProp_.itemID = INVALID_ITEM;

	return i1;

}
