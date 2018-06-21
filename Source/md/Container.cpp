#include <md/Container.hpp>

namespace md {

	Container::Container() {

	}
	Container::Container(std::string set_name) : MeteorItem(set_name) {

	}
	Container::~Container() {
		std::cout << "Destroying Container: " << this->name() << std::endl;
	}

	int Container::setParent(md::Container* set_parent) {
		if(this->parent_ptr == nullptr) {
			this->parent_ptr = set_parent;
			set_parent->add(this);
			return 1;
		} return 0;
	}

	md::MeteorItem* Container::add(md::MeteorItem* add_item) {
		if(add_item->getParent() == nullptr) {
			this->child_vctr.push_back(add_item);
			add_item->setParent(this);
			return add_item;
		} else if(add_item->getParent() == this) {
			if(this->child_vctr.size() && this->child_vctr.back() == add_item) return nullptr;
			this->child_vctr.push_back(add_item);
			return add_item;
		} return nullptr;
	}

	md::MeteorItem* Container::at(int i) {
		if(0 <= i && i < this->size()) {
			return this->child_vctr[i];
		} else if(-this->size() <= i && i < 0) {
			return this->child_vctr[this->size() + i];
		} else {
			return nullptr;
		}
	}

	int Container::remove(int i) {
		if(0 <= i && i < this->size()) {
			this->child_vctr.erase(this->child_vctr.begin() + i);
			return 1;
		} else if(-this->size() <= i && i < 0) {
			this->child_vctr.erase(this->child_vctr.begin() + this->size() + i);
			return 1;
		} else {
			return 0;;
		}
	}

	md::Type Container::getParentType() {
		if(this->parent_ptr == nullptr) {
			return md::Type::DNE;
		} else {
			return this->parent_ptr->getType();
		}
	}

	uint8_t Container::getParentTypeVal() {
		return static_cast<uint8_t>(this->getParentType());
	}

	size_t Container::size() {
		return this->child_vctr.size();
	}

}