#include <md/Container.hpp>

namespace md {
	Container::Container() {

	}
	Container::Container(std::string set_name) : MeteoroidObject(set_name) {

	}
	Container::Container(std::string set_name, md::Container* set_parent) : MeteoroidObject(set_name, set_parent) {

	}

	md::MeteoroidObject* Container::at(int i) {
		if(i < child_vctr.size()) {
			return child_vctr[i];
		} else {
			return nullptr;
		}
	}

	size_t Container::size() {
		return child_vctr.size();
	}
}