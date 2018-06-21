#pragma once

#include <md/MeteorItem.hpp>

namespace md {

	//This is the abstract object for any kind of container object (Folder, Group)
	class Container: public md::MeteorItem {
		protected:
			//Vector containing all children
			std::vector<md::MeteorItem*> child_vctr;

		public:
			//Only Containers can contain
			Container();
			Container(std::string);
			virtual ~Container();

			virtual int setParent(md::Container*) override;

			virtual md::MeteorItem* add(md::MeteorItem*);

			md::MeteorItem* at(int);

			virtual int remove(int);

			md::Type getParentType() override;

			uint8_t getParentTypeVal() override;

			size_t size();


	};

}