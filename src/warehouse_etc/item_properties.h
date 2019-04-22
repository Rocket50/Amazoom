#ifndef AMAZOOM_WAREHOUSE_ETC_ITEM_PROPERTIES_H_
#define AMAZOOM_WAREHOUSE_ETC_ITEM_PROPERTIES_H_

namespace amazoom {
	struct ItemProperties {
		int itemID; //number to uniquely identify item category
		float weight; //some bunches of bananas might be lighter than others
		bool isLarge; //some items might be difficult to handle due to size, packaging or other problems,
					  //may need to revisit this in the futurea

	};
}
#endif 