#pragma once

#include <md/Executable.hpp>

#include <md/Folder.hpp>
#include <md/Group.hpp>

//File system library
namespace fs = std::experimental::filesystem;

namespace md {
	class File: public md::Executable {
		private:
			fs::path file_path;
		public:
			File();
			File(std::string);
			File(std::string, fs::path);
			File(std::string, md::Container*);
			File(std::string, fs::path, md::Container*);

			md::Type getType() override;

			int setFilePath(fs::path);

			int setParent(md::MeteoroidObject*) override;

			void run() override;

	};
}