#include <md/Folder.hpp>

namespace md {

	Folder::Folder() {

	}
	Folder::Folder(std::string set_name) : Container(set_name), MeteoroidObject(set_name) {

	}
	Folder::Folder(std::string set_name, md::Folder* set_parent) : Container(set_name, set_parent), MeteoroidObject(set_name, set_parent) {
		set_parent->add(this);
	}

	//Folder can contain any kind of Object
	int Folder::add(md::MeteoroidObject* new_obj) {
		switch(new_obj->getType()) {
		case md::Type::Folder:
		case md::Type::Group:
		case md::Type::File:
			this->child_vctr.push_back(new_obj);
			new_obj->setParent(this);
			return 1;
		default:
			return 0;
		}
	}

	//Get Type
	md::Type Folder::getType() {
		return md::Type::Folder;
	}

	//Folders can only be children of parents
	int Folder::setParent(md::MeteoroidObject* set_parent) {
		if(set_parent->getType() == md::Type::Folder) {
			this->parent_ptr = set_parent;
			return 1;
		}
		return 0;
	}
}