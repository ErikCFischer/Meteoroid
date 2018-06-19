#pragma once

#include <md/MeteoroidObject.hpp>

namespace md {
	//This is the abstract object for any kind of container object (Folder, Group)
	class Container: virtual public md::MeteoroidObject {
		protected:
			//Vector containing all children
			std::vector<md::MeteoroidObject*> child_vctr;
		public:
			//Only Containers can contain
			Container();
			Container(std::string);
			Container(std::string, md::Container*);

			//This is just a quick way to add an object to the container
			virtual int add(md::MeteoroidObject*) = 0;

			//This is how to access any child object
			virtual md::MeteoroidObject* at(int );

			virtual size_t size();


	};
}