#pragma once

#include <md/Container.hpp>

namespace md {

	class Folder: public md::Container {
		public:
			Folder();
			Folder(std::string);
			virtual ~Folder();

			int setParent(md::Container*) override;

			md::MeteorItem* add(md::MeteorItem*) override;


	};

}