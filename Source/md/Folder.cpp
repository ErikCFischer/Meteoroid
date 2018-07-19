#include <MD/Folder.hpp>

namespace md {

	Folder::Folder() {
		this->type = md::Type::Folder;
	}
	Folder::Folder(std::string set_name) : Container(set_name) {
		this->type = md::Type::Folder;
	}
	Folder::~Folder() {
		std::cout << "Destroying Folder: " << this->name() << std::endl;
	}

	//Folder can contain any kind of Object
	int Folder::setParent(md::Container* set_parent) {
		if((this->parent_ptr == nullptr) && (set_parent->getType() == md::Type::Folder)) {
			this->parent_ptr = set_parent;
			set_parent->add(this);
			return 1;
		} return 0;
	}

	//Folder can contain any kind of Object
	md::MeteorItem* Folder::add(md::MeteorItem* add_item) {
		if(add_item->getParent() == nullptr) {
			switch(add_item->getType()) {
				case md::Type::Folder:
				case md::Type::Playlist:
				case md::Type::File:
					this->child_vctr.push_back(add_item);
					add_item->setParent(this);
					return add_item;
				default:
					return nullptr;
			}
		} else if(add_item->getParent() == this) {
			if(this->size() && this->child_vctr.back() == add_item) return nullptr;
			this->child_vctr.push_back(add_item);
			return add_item;
		} return nullptr;
	}
}