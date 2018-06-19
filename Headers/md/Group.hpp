#pragma once

#include <md/Container.hpp>
#include <md/Executable.hpp>

#include <md/Folder.hpp>

namespace md {
	class Group: public md::Container, public md::Executable {
		public:
			Group();
			Group(std::string);
			Group(std::string, md::Container*);

			int add(md::MeteoroidObject*) override;

			md::Type getType() override;

			int setParent(md::MeteoroidObject*) override;

			void run() override;

	};
}