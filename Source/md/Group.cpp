#include <md/Group.hpp>

namespace md {
	Group::Group() {

	}
	Group::Group(std::string set_name) : Container(set_name), MeteoroidObject(set_name) {

	}
	Group::Group(std::string set_name, md::Container* set_parent) : Container(set_name, set_parent), MeteoroidObject(set_name, set_parent) {
		set_parent->add(this);
	}

	int Group::add(md::MeteoroidObject* new_obj) {
		switch(new_obj->getType()) {
		case md::Type::Group:
		case md::Type::File:
			this->child_vctr.push_back(new_obj);
			new_obj->setParent(this);
			return 1;
		default:
			return 0;
		}
	}

	md::Type Group::getType() {
		return md::Type::Group;
	}

	int Group::setParent(md::MeteoroidObject* set_parent) {
		if(set_parent->getType() == md::Type::Folder || set_parent->getType() == md::Type::Group) {
			this->parent_ptr = set_parent;
			return 1;
		}
		return 0;
	}

	void Group::run() {
		
	}
}