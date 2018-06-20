#pragma once

#include <iostream>

//C++ Standard Library
#include <vector>
#include <string>

namespace md {

	//Different types of objects, this is basically a list of all the Meteoroid object types, may do this differently later
	enum class Type: uint8_t {DNE = 0, File = 1, Folder = 2, Playlist = 3};

	class Container;

	//Base class for a Meteoroid object, Abstract
	//This is what all types of Meteoroid objects have in common (e.g. a name, a parent, a type)
	class MeteorItem {
		friend class Folder;
		friend class Playlist;
		friend class File;
		protected:
			//Name
			std::string name_str;

			//Pointer to parent object
			md::Container* parent_ptr = nullptr;

			//Type
			md::Type type = md::Type::DNE;

		public:
			//Constructors
			//Set Parent, Name, or both
			MeteorItem();
			MeteorItem(std::string);
			virtual ~MeteorItem();

			virtual int setName(std::string);

			virtual int setParent(md::Container*) = 0;

			virtual std::string name();

			virtual md::Container* getParent();

			virtual md::Type getType();

			virtual uint8_t getTypeVal();

			virtual md::Type getParentType() = 0;

			virtual uint8_t getParentTypeVal() = 0;

	};

}