#include <md/MeteoroidObject.hpp>

namespace md {
	MeteoroidObject::MeteoroidObject() {

	}
	MeteoroidObject::MeteoroidObject(std::string set_name) : name_str(set_name) {

	}
	MeteoroidObject::MeteoroidObject(std::string set_name, md::MeteoroidObject* set_parent) : name_str(set_name), parent_ptr(set_parent) {

	}

	//Set name
	int MeteoroidObject::setName(std::string set_name) {
		name_str = set_name;
		return 1;
	}

	//Return the type of object the parent is
	//If no parent (root folder) returns DNE type
	md::Type MeteoroidObject::getType() {
		return md::Type::DNE;
	}

	uint8_t MeteoroidObject::getTypeVal() {
		return static_cast<uint8_t>(getType());
	}

	//Return the type of object the parent is
	//If no parent (root folder) returns DNE type
	md::Type MeteoroidObject::getParentType() {
		if(parent_ptr == nullptr) {
			return md::Type::DNE;
		} else {
			return parent_ptr->getType();
		}
	}

	uint8_t MeteoroidObject::getParentTypeVal() {
		return static_cast<uint8_t>(getParentType());
	}

	md::MeteoroidObject* MeteoroidObject::getParent() {
		if(parent_ptr == nullptr) {
			return nullptr;
		} else {
			return parent_ptr;
		}
	}

	//Get name
	std::string MeteoroidObject::name() {
		return name_str;
	}
}