#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <ctime>
#include <random>
#include <functional>

#include "warehouse_etc/item_definition.h"
#include "containers/multi_hashmap.h"

#include "unit_tests.h"

#include "boost/thread.hpp"



namespace AmazoomUnitTests
{		

	TEST_CLASS(Item_Testing) {
	public:


	//create and checks whether item is created with the right values
	TEST_METHOD(CheckValue) {
		const int itemID = 1;
		const float weight = 12.0f;
		amazoom::Item i(itemID, weight, true);
		checkItemEquals(i, itemID, weight, true);

		const int itemID2 = 1;
		const float weight2 = 12.0f;
		const bool isLarge2 = false;
		amazoom::Item i2(itemID, weight, isLarge2);
		checkItemEquals(i2, itemID2, weight2, isLarge2);
	}

	void Item_PassByValue(amazoom::Item i) {}
	//creates and checks constructor rules
	TEST_METHOD(CheckCopyCon) {

		const int itemID1 = 1;
		const float weight1 = 12.0f;

		amazoom::Item oldItem(itemID1, weight1);
		Item_PassByValue(std::move(oldItem));

		checkItemIsInvalid(oldItem);

	}

	TEST_METHOD(CheckCopyAssign) {

		const int itemID1 = 1;
		const float weight1 = 12.0f;

		amazoom::Item oldItem(itemID1, weight1);
		amazoom::MultiHashmap<int, amazoom::Item> bob;
	}

	TEST_METHOD(OverloadedCompares) {

		const int itemID1 = 1;
		const float weight1 = 12.0f;

		amazoom::Item item(itemID1, weight1);
		amazoom::Item item2(itemID1, weight1);
		
		 
		bool isEqual = (item == item2);
		Assert::IsTrue(isEqual);
		const int itemID2 = 5;
		
		amazoom::Item item3(itemID2, weight1);
		isEqual = (itemID2 == weight1);

		Assert::IsFalse(isEqual);

	};

	};
};

	TEST_CLASS(Mult_Hashmap_Testing) {

		TEST_METHOD(SingleInsertion) {
			
			const float weight(12.0f);

			amazoom::Item item(5, weight);
			amazoom::Item item2(5, weight);

			amazoom::MultiHashmap<int, amazoom::Item> container;

			container.insertItem(item.getID(), item);
			Assert::AreEqual(1, container.getNumItems());
			//check if old item was invalidated after insertion
			checkItemIsInvalid(item);

			container.insertItem(item2.getID(), item2);

			//check if the container grew as we expected
			Assert::AreEqual(2, container.getNumItems());

			//check if old item was invalidated after insertion
			checkItemIsInvalid(item2);
		};

		TEST_METHOD(DoubleExtractionSameID) {
			const int id = 5;
			const float weight1 = 1.0f;
			const float weight2 = 10.0f;

			amazoom::Item item(id, weight1);
			amazoom::Item item2(id, weight2);

			amazoom::MultiHashmap<int, amazoom::Item> container;

			container.insertItem(item.getID(), item);
			container.insertItem(item2.getID(), item2);

			//items should be invalid after insertion
			checkItemIsInvalid(item);
			checkItemIsInvalid(item2);
			Assert::AreEqual(2, container.getNumItems());

			amazoom::Item extractedItem(container.extractItem(id));

			checkItemEquals(extractedItem, id, weight2);
			amazoom::Item extractedItem2(container.extractItem(id));

			checkItemEquals(extractedItem2, id, weight1);
			Assert::AreEqual(0, container.getNumItems());

		};

		TEST_METHOD(DoubleExtractionDifferentID) {
			const int id1 = 5;
			const int id2 = 2015;
			const float weight1 = 1.0f;
			const float weight2 = 10.0f;

			amazoom::Item item1(id1, weight1);
			amazoom::Item item2(id2, weight2);

			amazoom::MultiHashmap<int, amazoom::Item> container;

			container.insertItem(item1.getID(), item1);
			container.insertItem(item2.getID(), item2);

			//items should be invalid after insertion
			checkItemIsInvalid(item1);
			checkItemIsInvalid(item2);
			Assert::AreEqual(2, container.getNumItems());

			amazoom::Item extractedItem(container.extractItem(id1));

			checkItemEquals(extractedItem, id1, weight1);
			amazoom::Item extractedItem2(container.extractItem(id2));

			checkItemEquals(extractedItem2, id2, weight2);
			Assert::AreEqual(0, container.getNumItems());

		};

		
		TEST_METHOD(RandomExtractionSpecial) {
			const int NUMTOTEST = 10000;

			std::random_device rd; // obtain a random number from hardware
			std::mt19937 eng(rd()); // seed the generator

			std::vector<std::pair<int, float>> itemCreateVals;

			const int MAX_RAND_ID = 50;
			const float MAX_WEIGHT = 10.0f;

			amazoom::MultiHashmap<int, amazoom::Item> hashmap;


			auto idRandomizer = std::uniform_int_distribution<int>(0, MAX_RAND_ID);
			for (int i = 0; i < NUMTOTEST; i++) {

				int randID(idRandomizer(eng));

				float randWeight(1 + std::rand() / ((RAND_MAX + 1u) / MAX_WEIGHT));


				itemCreateVals.push_back(
					std::make_pair<int, float>(std::move(randID), std::move(randWeight)));

				amazoom::Item newItem(randID, randWeight);
				hashmap.insertItem(newItem.getID(), newItem);
			}


			for (int i = 0; i < NUMTOTEST; i++) {

				auto indexRandomizer = std::uniform_int_distribution<int>(0, itemCreateVals.size() - 1);
				const int randIndex = indexRandomizer(eng);

				std::pair<int, float> toRetrieve = itemCreateVals.at(randIndex);
				const int id = std::get<int>(toRetrieve);
				const float weight = std::get<float>(toRetrieve);

				itemCreateVals.erase(itemCreateVals.begin() + randIndex);

				auto comparisonFuncWeightID = [weight](const amazoom::Item& item)->bool {
					return item.getWeight() == weight;
				};

				amazoom::Item itemRetrieved(hashmap.extractItem(id, comparisonFuncWeightID));
				checkItemEquals(itemRetrieved, id, weight);
			}
		};

		TEST_METHOD(RandomIDOnly) {

			const int NUMTOTEST = 10000;

			std::srand(std::time(nullptr)); // use current time as seed for random generator
			std::random_device rd; // obtain a random number from hardware
			std::mt19937 eng(rd()); // seed the generator

			std::vector<std::pair<int, float>> itemCreateVals;

			const int MAX_RAND_ID = 20;
			const float MAX_WEIGHT = 10.0f;

			amazoom::MultiHashmap<int, amazoom::Item> hashmap;

			for (int i = 0; i < NUMTOTEST; i++) {
				auto idRandomizer = std::uniform_int_distribution<int>(0, MAX_RAND_ID);
				int randID(idRandomizer(eng));

				float randWeight(1 + std::rand() / ((RAND_MAX + 1u) / MAX_WEIGHT));


				itemCreateVals.push_back(
					std::make_pair<int, float>(std::move(randID), std::move(randWeight)));

				amazoom::Item newItem(randID, randWeight);
				hashmap.insertItem(newItem.getID(), newItem);
			}

			for (int i = 0; i < NUMTOTEST; i++) {

				auto idRandomizer = std::uniform_int_distribution<int>(0, itemCreateVals.size() - 1);
				int randID(idRandomizer(eng));

				std::pair<int, float> toRetrieve = itemCreateVals.at(randID);
				const int id = std::get<int>(toRetrieve);
				const float weight = std::get<float>(toRetrieve);

				itemCreateVals.erase(itemCreateVals.begin() + randID);

				amazoom::Item itemRetrieved(hashmap.extractItem(id));
				checkItemEquals(itemRetrieved, id, itemRetrieved.getWeight());



			
			}
		};


		TEST_METHOD(DoesContainObjectNormal) {
			const int ID = 20;
			const float WEIGHT = 10.0f;

			amazoom::Item newItem(ID, WEIGHT);
			amazoom::MultiHashmap<int, amazoom::Item> container;

			container.insertItem(newItem.getID(), newItem);

			Assert::IsTrue(container.doesContainObj(ID));
			Assert::IsFalse(container.doesContainObj(ID+1));

			const int ID2 = 54;
			const float WEIGHT2 = 100.0f;
			amazoom::Item newItem2(ID2, WEIGHT2);

			container.insertItem(newItem2.getID(), newItem2);

			Assert::IsTrue(container.doesContainObj(ID2));
			Assert::IsFalse(container.doesContainObj(ID2 + 1));

		};

		TEST_METHOD(DoesContainObjectSpecial) {
			const int ID = 20;
			const float WEIGHT = 10.0f;

			amazoom::Item newItem(ID, WEIGHT);
			amazoom::MultiHashmap<int, amazoom::Item> container;

			auto comparisonFunc = [](const amazoom::Item& item)->bool {return true; };

			container.insertItem(newItem.getID(), newItem);

			Assert::IsTrue(container.doesContainObj(ID, comparisonFunc));
			Assert::IsFalse(container.doesContainObj(ID+1, comparisonFunc));

			auto comparisonFuncWeightID = [ID, WEIGHT](const amazoom::Item& item)->bool {
			
				return item.getID() == ID && item.getWeight() == WEIGHT;

			};

			Assert::IsTrue(container.doesContainObj(ID, comparisonFuncWeightID));

			container.extractItem(ID);

			Assert::IsFalse(container.doesContainObj(ID, comparisonFuncWeightID));
		};

		TEST_METHOD(ExtractionSpecial) {
			const int id1 = 5;
			const int id2 = 2015;
			const float weight1 = 1.0f;
			const float weight2 = 10.0f;

			amazoom::Item item1(id1, weight1);
			amazoom::Item item2(id2, weight2);

			amazoom::MultiHashmap<int, amazoom::Item> container;

			container.insertItem(item1.getID(), item1);
			container.insertItem(item2.getID(), item2);

			//items should be invalid after insertion
			checkItemIsInvalid(item1);
			checkItemIsInvalid(item2);
			Assert::AreEqual(2, container.getNumItems());

			//extract by an enhanced filter, specialty: weight
			auto comparisonFuncWeightID = [id2, weight2](const amazoom::Item& item)->bool {
				return item.getWeight() == weight2;
			};


			amazoom::Item extractedItem(container.extractItem(id2, comparisonFuncWeightID));

			checkItemEquals(extractedItem, id2, weight2);

			amazoom::Item extractedItem2(container.extractItem(id1));

			checkItemEquals(extractedItem2, id1, weight1);
			Assert::AreEqual(0, container.getNumItems());
		};

		static void multithreadingInsert(const int NUM_INSERTIONS, 
			amazoom::MultiHashmap<int, amazoom::Item>& hashmap, 
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

		TEST_METHOD(RandomMultithreading) {

			const int NUM_TO_TEST_PER_THREAD = 3000;
			const int THREADS = 4;


			std::srand(std::time(nullptr)); // use current time as seed for random generator
			std::random_device rd; // obtain a random number from hardware
			std::mt19937 eng(rd()); // seed the generator

			std::vector<std::pair<int, float>> itemCreateVals;



			amazoom::MultiHashmap<int, amazoom::Item> hashmap;

			std::vector<std::pair<int, float>> stuff[THREADS];

			std::vector<std::unique_ptr<boost::thread>> threads;

			//create empty threads, not started
			for (int i = 0; i < THREADS; i++) {
				threads.push_back(
					std::unique_ptr<boost::thread>());


			}

			//start each thread
			for (int i = 0; i < THREADS; i++) {

				threads.at(i).reset(((new boost::thread(multithreadingInsert, NUM_TO_TEST_PER_THREAD, std::ref(hashmap), std::ref(stuff[i]), std::ref(eng)))));

			}

			//wait for each thread to finish its job
			for (int i = 0; i < THREADS; i++) {
				threads.at(i)->join();
			}


			for (int i = 0; i < THREADS; i++) {
				for (unsigned int j = 0; j < NUM_TO_TEST_PER_THREAD; j++) {
					itemCreateVals.push_back(stuff[i].back());
					stuff[i].pop_back();
				}

			}



			for (int i = 0; i < (NUM_TO_TEST_PER_THREAD*THREADS); i++) {

				auto idRandomizer = std::uniform_int_distribution<int>(0, itemCreateVals.size() - 1);
				int randID(idRandomizer(eng));

				std::pair<int, float> toRetrieve = itemCreateVals.at(randID);
				const int id = std::get<int>(toRetrieve);
				const float weight = std::get<float>(toRetrieve);

				itemCreateVals.erase(itemCreateVals.begin() + randID);

				auto comparisonFuncWeightID = [weight](const amazoom::Item& item)->bool {
					return item.getWeight() == weight;
				};

				amazoom::Item itemRetrieved(hashmap.extractItem(id, comparisonFuncWeightID));

				checkItemEquals(itemRetrieved, id, weight);

			}
		};
	};

