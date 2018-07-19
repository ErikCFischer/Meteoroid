#pragma once

//C++ Standard Library
#include <iostream>
#include <vector>
#include <string>
//SQLite Library
#include <sqlite3.h>
//SFML
#include <SFML/Graphics.hpp>
//Local includes
#include <MD/Folder.hpp>
#include <MD/Playlist.hpp>
#include <MD/File.hpp>

class VirtualSystemScreen : public sf::Drawable {
	private:
		float left, top, width, height;

		sf::Font font;
		std::vector<sf::Text> virtual_path_text;
		std::vector<sf::Text> virtual_item_text;

		sf::Vector2f cursor_pos;

		sqlite3* db;
		md::MeteorItem* current_item;

		virtual void draw(sf::RenderTarget&, sf::RenderStates) const;

	public:
		VirtualSystemScreen(float,float,float,float);
		~VirtualSystemScreen();
		
		void updateCursorPos(sf::Vector2f);
		void updatePath();


};

//Struct to hold an SQL table
struct sql_table_s {
	std::vector<std::string> column_name;
	std::vector<std::vector<std::string>> column_data;
	std::vector<size_t> max_column_width;
};

void loadMDFromDB(sqlite3*, md::MeteorItem*, int);

static int callback(void* table_output, int col_count, char** col_data, char** col_name) {

	sql_table_s* out = static_cast<sql_table_s*>(table_output);

	if( !(*out).column_name.size() ) {
		for(int col = 0; col < col_count; col++) {
			(*out).column_name.push_back(col_name[col]);
			(*out).max_column_width.push_back(strlen(col_name[col]));
		}
	}

	std::vector<std::string> row_data;
	for(int col = 0; col < col_count; col++) {
		if(!col_data[col])
			row_data.push_back("NULL");
		else
			row_data.push_back(col_data[col]);

		if((*out).max_column_width[col] < strlen(row_data.back().c_str())) {
			(*out).max_column_width[col] = strlen(row_data.back().c_str());
		}
	}
	(*out).column_data.push_back(row_data);

	return 0;

}