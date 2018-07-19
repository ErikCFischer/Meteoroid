#include <Screens/VirtualSystemScreen.hpp>

#ifdef DEBUG
	#define dbp(x) std::cout << x << std::endl
#else
	#define dbp(x) //x
#endif

VirtualSystemScreen::VirtualSystemScreen(float set_left, float set_top, float set_width, float set_height) {

	left = set_left;
	top = set_top;
	width = set_width;
	height = set_height;

	char* sql_err = 0;
	int sql_status;

	sql_status = sqlite3_open("test.db", &db);

	if( sql_status != SQLITE_OK ) {

		dbp("Can't open database: " << sqlite3_errmsg(db));

	} else {

		dbp("Opened database successfully");

	}
	
	char* sql_statement = "SELECT * FROM md_item WHERE ID = 0;";

	sql_table_s md_table;

	/* Execute SQL statement */
	sql_status = sqlite3_exec(db, sql_statement, callback, &md_table, &sql_err);

	if( sql_status != SQLITE_OK ){
		dbp("SQL error: " << sql_err);
		sqlite3_free(sql_err);
	} else {
		dbp("Loaded Virtual Root");
	}

	current_item = new md::Folder(md_table.column_data[0][1]);

	loadMDFromDB(db, current_item, 0);
	dbp("Virtual System Loaded");

	sf::Color folder_fill_c(0xA0, 0xA0, 0xF0);
	sf::Color folder_outline_c(0x40, 0x40, 0x60);

	sf::Color playlist_fill_c(0xA0, 0xF0, 0xA0);
	sf::Color playlist_outline_c(0x40, 0x60, 0x40);

	sf::Color file_fill_c(0xF0, 0xA0, 0xA0);
	sf::Color file_outline_c(0x60, 0x40, 0x40);

	virtual_path_text.clear();
	virtual_path_text.emplace_back(current_item->name(), font, 16);
	virtual_path_text.back().setFillColor(folder_fill_c);
	virtual_path_text.back().setOutlineColor(folder_outline_c);
	virtual_path_text.back().setPosition(left, top);

	md::Container* current_container = dynamic_cast<md::Container*>(current_item);

	for(int i = 0; i < current_container->size(); i++) {

		virtual_item_text.emplace_back(current_container->at(i)->name(), font, 16);
		virtual_item_text.back().setPosition(sf::Vector2f(left, top + 24 + 24*i));

		if(current_container->at(i)->getType() == md::Type::Folder) {
			virtual_item_text.back().setFillColor(folder_fill_c);
			virtual_item_text.back().setOutlineColor(folder_outline_c);
		} else if(current_container->at(i)->getType() == md::Type::Playlist) {
			virtual_item_text.back().setFillColor(playlist_fill_c);
			virtual_item_text.back().setOutlineColor(playlist_outline_c);
		} else if(current_container->at(i)->getType() == md::Type::File) {
			virtual_item_text.back().setFillColor(file_fill_c);
			virtual_item_text.back().setOutlineColor(file_outline_c);
		}

	}

	if(!font.loadFromFile("Assets/crystal.ttf")) {
		dbp("Error loading font");
	}

}

VirtualSystemScreen::~VirtualSystemScreen() {
	sqlite3_close(db);
}

void VirtualSystemScreen::updateCursorPos(sf::Vector2f cursor_pos_update) {

	cursor_pos = cursor_pos_update;

	for(auto & path_text : virtual_path_text) {
		if(path_text.getGlobalBounds().contains(cursor_pos)) {
			path_text.setOutlineThickness(1);
		} else if(path_text.getOutlineThickness() == 1) {
			path_text.setOutlineThickness(0);
		}
	}

	for(auto & item_text : virtual_item_text) {
		if(item_text.getGlobalBounds().contains(cursor_pos)) {
			item_text.setOutlineThickness(1);
		} else if(item_text.getOutlineThickness() == 1) {
			item_text.setOutlineThickness(0);
		}
	}

}

void VirtualSystemScreen::updatePath() {

	sf::Color folder_fill_c(0xA0, 0xA0, 0xF0);
	sf::Color folder_outline_c(0x40, 0x40, 0x60);

	sf::Color playlist_fill_c(0xA0, 0xF0, 0xA0);
	sf::Color playlist_outline_c(0x40, 0x60, 0x40);

	sf::Color file_fill_c(0xF0, 0xA0, 0xA0);
	sf::Color file_outline_c(0x60, 0x40, 0x40);

	for(auto & path_text : virtual_path_text) {
		if(path_text.getGlobalBounds().contains(cursor_pos)) {

			while(virtual_path_text.back().getString() != path_text.getString()) {
				current_item = current_item->getParent();
				virtual_path_text.pop_back();
			}

			if(md::Container* current_container = dynamic_cast<md::Container*>(current_item)) {

				virtual_item_text.clear();

				for(int i = 0; i < current_container->size(); i++) {

					virtual_item_text.emplace_back(current_container->at(i)->name(), font, 16);
					virtual_item_text.back().setPosition(sf::Vector2f(left, top + 24 + 24*i));

					if(current_container->at(i)->getType() == md::Type::Folder) {
						virtual_item_text.back().setFillColor(folder_fill_c);
						virtual_item_text.back().setOutlineColor(folder_outline_c);
					} else if(current_container->at(i)->getType() == md::Type::Playlist) {
						virtual_item_text.back().setFillColor(playlist_fill_c);
						virtual_item_text.back().setOutlineColor(playlist_outline_c);
					} else if(current_container->at(i)->getType() == md::Type::File) {
						virtual_item_text.back().setFillColor(file_fill_c);
						virtual_item_text.back().setOutlineColor(file_outline_c);
					}

				}

			}

			break;

		}
	}


	for(auto & item_text : virtual_item_text) {
		if(item_text.getGlobalBounds().contains(cursor_pos)) {

			if(md::Container* current_container = dynamic_cast<md::Container*>(current_item)) {

				current_item = current_container->at(int(item_text.getPosition().y - top) / 24 - 1);

				virtual_path_text.emplace_back("/" + current_item->name(), font, 16);
				if(current_item->getType() == md::Type::Folder) {
					virtual_path_text.back().setFillColor(folder_fill_c);
					virtual_path_text.back().setOutlineColor(folder_outline_c);
				} else if(current_item->getType() == md::Type::Playlist) {
					virtual_path_text.back().setFillColor(playlist_fill_c);
					virtual_path_text.back().setOutlineColor(playlist_outline_c);
				} else if(current_item->getType() == md::Type::File) {
					virtual_path_text.back().setFillColor(file_fill_c);
					virtual_path_text.back().setOutlineColor(file_outline_c);
				}

				virtual_path_text.back().setPosition(virtual_path_text[virtual_path_text.size() - 2].findCharacterPos(-1));

				virtual_item_text.clear();

				if(current_container = dynamic_cast<md::Container*>(current_item)) {

					for (int i = 0; i < current_container->size(); i++) {

						virtual_item_text.emplace_back(current_container->at(i)->name(), font, 16);
						virtual_item_text.back().setPosition(sf::Vector2f(left, top + 24 + 24*i));

						if(current_container->at(i)->getType() == md::Type::Folder) {
							virtual_item_text.back().setFillColor(folder_fill_c);
							virtual_item_text.back().setOutlineColor(folder_outline_c);
						} else if(current_container->at(i)->getType() == md::Type::Playlist) {
							virtual_item_text.back().setFillColor(playlist_fill_c);
							virtual_item_text.back().setOutlineColor(playlist_outline_c);
						} else if(current_container->at(i)->getType() == md::Type::File) {
							virtual_item_text.back().setFillColor(file_fill_c);
							virtual_item_text.back().setOutlineColor(file_outline_c);
						}

						if(virtual_item_text.back().getGlobalBounds().contains(cursor_pos)) {
							virtual_item_text.back().setOutlineThickness(1);
						}

					}

				} else {
					dbp("File");
				}
				
			} else {
				dbp("File");
			}

			break;
			
		}
	}

}

void VirtualSystemScreen::draw(sf::RenderTarget& target, sf::RenderStates states) const {

	for (auto path_text : virtual_path_text) {
		target.draw(path_text, states);
	}

	for (auto item_text : virtual_item_text) {
		target.draw(item_text, states);
	}

}



void loadMDFromDB(sqlite3* db, md::MeteorItem* md, int id) {

	std::string sql_statement = "SELECT * FROM md_item WHERE parent_ID = " + std::to_string(id) + ";";
	sql_table_s md_table;
	char* unused;
	sqlite3_exec(db, sql_statement.c_str(), callback, &md_table, &unused);

	for(int row = 0; row < md_table.column_data.size(); row++) {
		if(md_table.column_data[row][3] == "Folder") {

			loadMDFromDB(db,
						static_cast<md::Container*>(md)->add(new md::Folder(md_table.column_data[row][1])),
						std::stoi(md_table.column_data[row][0]));

		} else if(md_table.column_data[row][3] == "Playlist") {

			loadMDFromDB(db,
						static_cast<md::Container*>(md)->add(new md::Playlist(md_table.column_data[row][1])),
						std::stoi(md_table.column_data[row][0]));

		} else if(md_table.column_data[row][3] == "File") {

			static_cast<md::Container*>(md)->add(new md::File(md_table.column_data[row][1]));

		}
	}

}