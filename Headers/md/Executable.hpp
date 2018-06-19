#pragma once

#include <md/Container.hpp>

namespace md {
	//This is the abstract object for any kind of executable functionality (Group, File)
	class Executable: virtual public md::MeteoroidObject {
		protected:
			//This is the str that hold the command to run
			std::string command_str;
		public:
			Executable();
			Executable(std::string);
			Executable(std::string, md::Container*);

			//Set command
			virtual int setCommand(std::string);

			//Get command
			virtual std::string command();

			//Individually implimented function controlling how to run the Executable
			virtual void run() = 0;

	};
}