#include <md/MeteorItem.hpp>

namespace md {

	MeteorItem::MeteorItem() {

	}
	MeteorItem::MeteorItem(std::string set_name) : name_str(set_name) {

	}
	MeteorItem::~MeteorItem() {
		std::cout << "Destroying MeteorItem: " << this->name() << std::endl;
	}

	//Set name
	int MeteorItem::setName(std::string set_name) {
		this->name_str = set_name;
		return 1;
	}

	//Get name
	std::string MeteorItem::name() {
		return this->name_str;
	}

	md::Container* MeteorItem::getParent() {
		return this->parent_ptr;
	}

	md::Type MeteorItem::getType() {
		return this->type;
	}

	uint8_t MeteorItem::getTypeVal() {
		return static_cast<uint8_t>(this->type);
	}

}