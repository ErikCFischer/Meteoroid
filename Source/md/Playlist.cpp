#include <MD/Playlist.hpp>

namespace md {

	Playlist::Playlist() {
		this->type = md::Type::Playlist;
	}
	Playlist::Playlist(std::string set_name) : Container(set_name) {
		this->type = md::Type::Playlist;
	}
	Playlist::~Playlist() {
		std::cout << "Destroying Playlist: " << this->name() << std::endl;
		this->child_vctr.clear();
	}

	int Playlist::setParent(md::Container* set_parent) {
		if(this->parent_ptr == nullptr) {
			switch(set_parent->getType()) {
				case md::Type::Folder:
				case md::Type::Playlist:
					this->parent_ptr = set_parent;
					set_parent->add(this);
					return 1;
				default:
					return 0;
			}
		} return 0;
	}

	md::MeteorItem* Playlist::add(md::MeteorItem* add_item) {
		if(add_item->getParent() == nullptr) {
			switch(add_item->getType()) {
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

	void Playlist::run() {
		
	}

}