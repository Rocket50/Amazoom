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

static void multithreadingInsert(const int NUM_INSERTIONS,
	amazoom::WorkerAccessibleContainer& hashmap,
	std::vector<std::pair<int, float>>& results,
	std::mt19937& eng) {

	const int MAX_RAND_ID = 20;
	const float MAX_WEIGHT = 10.0f;

	for (int i = 0; i < NUM_INSERTIONS; i++) {
		auto idRandomizer = std::uniform_int_distribution<int>(0, MAX_RAND_ID);
		int randID(idRandomizer(eng));

		float randWeight(1 + std::rand() / ((RAND_MAX + 1u) / MAX_WEIGHT));

		results.push_back(
			std::make_pair<int, float>(std::move(randID), std::move(randWeight)));

		amazoom::Item newItem(randID, randWeight);
		hashmap.insertItem(newItem.getID(), newItem);

	}
}
//returns:
//1. vector containing state information of generated items
//2. Items generated
std::pair<
	std::vector<amazoom::ItemProperties>,
	std::vector<amazoom::Item>
>
getRandomItems(const int NUM_TO_MAKE,
			   const int MIN_ID,
			   const int MAX_ID,
		   	   const float MIN_WEIGHT,
			   const float MAX_WEIGHT) {

	typedef std::vector<amazoom::ItemProperties> ItemProperties;
	typedef std::vector<amazoom::Item> Items;


	std::srand(std::time(nullptr)); // use current time as seed for random generator
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 eng(rd()); // seed the generator

	auto idRandomizer = std::uniform_int_distribution<int>(MIN_ID, MAX_ID);

	ItemProperties propertiesGenerated;
	Items itemsGenerated;

	for (int i = 0; i < NUM_TO_MAKE; i++) {

		float randWeight((MAX_WEIGHT - MIN_WEIGHT) * ((((float)rand()) / (float)RAND_MAX)) + MIN_WEIGHT);

		amazoom::ItemProperties genProp{ idRandomizer(eng) , randWeight, false };
		propertiesGenerated.push_back(genProp);
		itemsGenerated.push_back(std::move(amazoom::Item(genProp)));
	}

	std::pair<ItemProperties, Items> genObj(std::make_pair(propertiesGenerated, std::move(itemsGenerated)));
	return std::move(genObj);

}


