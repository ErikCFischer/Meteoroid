#pragma once

#include <iostream>


//C++ Standard Library
#include <vector>
#include <string>
//C++17 Experimental Libraries
#include <experimental/filesystem>

namespace md {

	//Different types of objects, this is basically a list of all the Meteoroid object types, may do this differently later
	enum class Type: uint8_t {DNE = 0, File = 1, Folder = 2, Group = 3};

	//Base class for a Meteoroid object, Abstract
	//This is what all types of Meteoroid objects have in common (e.g. a name, a parent, a type)
	class MeteoroidObject {
		friend class Folder;
		friend class Group;
		friend class File;
		protected:
			//Pointer to parent object
				//Actual Parent objects are left up to implementation
				//E.G. Files cannot be parents, each object will have to state what type of object can be set as a parent
			md::MeteoroidObject* parent_ptr = nullptr;

			//Name
			std::string name_str;

			//Constructors
			//Set Parent, Name, or both

			//Set parent
			virtual int setParent(md::MeteoroidObject*) = 0;

		public:

			MeteoroidObject();
			MeteoroidObject(std::string);
			MeteoroidObject(std::string, md::MeteoroidObject*);

			//Set name
			virtual int setName(std::string);

			//This will return the type of object it is generically
			//This may eventually have a better solution, but it works for now
			virtual md::Type getType();

			virtual uint8_t getTypeVal();

			//Return the type of object the parent is
			//If no parent (root folder) returns DNE type
			virtual md::Type getParentType();

			virtual uint8_t getParentTypeVal();

			virtual md::MeteoroidObject* getParent();

			//Get name
			virtual std::string name();

	};

}