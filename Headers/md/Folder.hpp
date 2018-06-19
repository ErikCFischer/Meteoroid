#pragma once

#include <md/Container.hpp>

namespace md {
	class Folder: public md::Container {
		public:
			Folder();
			Folder(std::string);
			Folder(std::string, md::Folder*);

			//Folder can contain any kind of Object
			int add(md::MeteoroidObject*) override;

			//Get Type
			md::Type getType() override;

			//Folders can only be children of parents
			int setParent(md::MeteoroidObject*) override;
	};
}