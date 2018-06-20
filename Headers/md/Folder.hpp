#pragma once

#include <md/Container.hpp>

namespace md {

	class Folder: public md::Container {
		public:
			Folder();
			Folder(std::string);
			Folder(std::string, md::Folder*);
			virtual ~Folder();

			int setParent(md::Container*) override;

			int add(md::MeteorItem*) override;


	};

}