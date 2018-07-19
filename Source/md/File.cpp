#include <MD/File.hpp>

namespace md {

	File::File() {
		this->type = md::Type::File;
	}
	File::File(std::string set_name) : MeteorItem(set_name) {
		this->type = md::Type::File;
	}
	File::File(std::string set_name, fs::path set_path) : MeteorItem(set_name), file_path(set_path) {
		this->type = md::Type::File;
	}
	File::~File() {
		std::cout << "Destroying File: " << this->name() << std::endl;
	}

	int File::setParent(md::Container* set_parent) {
		if(this->parent_ptr == nullptr) {
			this->parent_ptr = set_parent;
			set_parent->add(this);
			return 1;
		} return 0;
	}

	md::Type File::getParentType() {
		if(this->parent_ptr == nullptr) {
			return md::Type::DNE;
		} else {
			return this->parent_ptr->getType();
		}
	}

	uint8_t File::getParentTypeVal() {
		return static_cast<uint8_t>(this->getParentType());
	}

	int File::setFilePath(fs::path set_path) {
		this->file_path = set_path;
		return 1;
	}

	void File::run() {

	}

}