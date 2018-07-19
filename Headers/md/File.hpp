#pragma once

//C++17 Experimental Libraries
#include <experimental/filesystem>

#include <MD/Container.hpp>

//File system library
namespace fs = std::experimental::filesystem;

namespace md {
	
	class File: public md::MeteorItem {
		private:
			fs::path file_path;
		public:
			File();
			File(std::string);
			File(std::string, fs::path);
			virtual ~File();

			int setParent(md::Container*) override;

			md::Type getParentType() override;

			uint8_t getParentTypeVal() override;

			int setFilePath(fs::path);

			void run();

	};

}