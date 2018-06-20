#pragma once

#include <md/Container.hpp>

namespace md {

	class Playlist: public md::Container {
		public:
			Playlist();
			Playlist(std::string);
			Playlist(std::string, md::Container*);
			virtual ~Playlist();

			int setParent(md::Container*) override;

			int add(md::MeteorItem*) override;

			void run();

	};

}