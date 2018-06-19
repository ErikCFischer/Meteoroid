#include <md/File.hpp>

namespace md {
	File::File() {

	}
	File::File(std::string set_name) : Executable(set_name), MeteoroidObject(set_name) {

	}
	File::File(std::string set_name, fs::path set_path) : Executable(set_name), MeteoroidObject(set_name), file_path(set_path) {

	}
	File::File(std::string set_name, md::Container* set_parent) : Executable(set_name, set_parent), MeteoroidObject(set_name, set_parent) {
		set_parent->add(this);
	}
	File::File(std::string set_name, fs::path set_path, md::Container* set_parent) : Executable(set_name, set_parent), MeteoroidObject(set_name, set_parent), file_path(set_path) {
		set_parent->add(this);
	}

	md::Type File::getType() {
		return md::Type::File;
	}

	int File::setFilePath(fs::path set_file_path) {
		file_path = set_file_path;
		return 1;
	}

	int File::setParent(md::MeteoroidObject* set_parent) {
		if(set_parent->getType() == md::Type::Folder || set_parent->getType() == md::Type::Group) {
			this->parent_ptr = set_parent;
			return 1;
		}
		return 0;
	}

	void File::run() {

	}
}