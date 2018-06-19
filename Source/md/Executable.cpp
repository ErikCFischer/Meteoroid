#include <md/Executable.hpp>

namespace md {
	Executable::Executable() {

	}
	Executable::Executable(std::string set_name) : MeteoroidObject(set_name) {

	}
	Executable::Executable(std::string set_name, md::Container* set_parent) : MeteoroidObject(set_name, set_parent) {

	}
	
	int Executable::setCommand(std::string set_command) {
		command_str = set_command;
		return 1;
	}

	//Get command
	std::string Executable::command() {
		return command_str;
	}
}