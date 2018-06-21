#pragma once

#include <md/Container.hpp>

namespace md {

	class Playlist: public md::Container {
		public:
			Playlist();
			Playlist(std::string);
			virtual ~Playlist();

			int setParent(md::Container*) override;

			md::MeteorItem* add(md::MeteorItem*) override;

			void run();

	};

}