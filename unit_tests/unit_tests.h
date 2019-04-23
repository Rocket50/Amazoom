#pragma once

#include "stdafx.h"
#include "CppUnitTest.h"

#include "warehouse_etc/item_definition.h"
#include "containers/multi_hashmap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void checkItemEquals(amazoom::Item& i, const int itemID, const float weight, const bool isLarge) {
	Assert::AreEqual(i.getID(), itemID);
	Assert::AreEqual(i.getWeight(), weight);
	Assert::AreEqual(i.isLarge(), isLarge);
}

void checkItemEquals(amazoom::Item& i, const int itemID, const float weight) {
	checkItemEquals(i, itemID, weight, i.isLarge());
}

void checkItemIsInvalid(amazoom::Item& i) {
	Assert::AreEqual(i.getID(), static_cast<int>(amazoom::Item::INVALID_ITEM));
}


