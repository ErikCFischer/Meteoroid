//C++ Standard Library
#include <iostream>
#include <vector>
#include <string>
//C++17 Experimental Libraries
#include <experimental/filesystem>
//C Standard Library
#include <stdio.h>
#include <math.h>
//SQLite Library
#include <sqlite3.h>
//SFML
#include <SFML/Graphics.hpp>

namespace fs = std::experimental::filesystem;

struct sql_table_s {
	std::vector<std::string> column_name;
	std::vector<std::vector<std::string>> column_data;
	std::vector<size_t> max_column_width;
};



namespace md {

	enum class Type {Folder, Group, File, DNE};

	class MeteoroidObject {
		protected:
			md::MeteoroidObject* parent = nullptr;

			MeteoroidObject() {}
			MeteoroidObject(md::MeteoroidObject* set_parent) : parent(set_parent) {}
			MeteoroidObject(std::string set_name) : name(set_name) {}
			MeteoroidObject(md::MeteoroidObject* set_parent, std::string set_name) : parent(set_parent), name(set_name) {}
		public:
			std::string name;

			virtual md::Type getType() = 0;

			virtual md::Type getParentType() {
				if(parent == nullptr) {
					return md::Type::DNE;
				} else {
					return parent->getType();
				}
			}

			virtual void setParent(md::MeteoroidObject* set_parent) {
				parent = set_parent;
			}

	};

	class Folder: public md::MeteoroidObject {
		private:
			std::vector<md::MeteoroidObject*> children;
		public:
			Folder() {}
			Folder(md::Folder* set_parent) : MeteoroidObject(set_parent) {}
			Folder(std::string set_name) : MeteoroidObject(set_name) {}
			Folder(md::Folder* set_parent, std::string set_name) : MeteoroidObject(set_parent, set_name) {}

			int operator+=(md::MeteoroidObject* new_obj) {
				if(new_obj->getType() == md::Type::DNE) {
					return 0;
				}
				children.push_back(new_obj);
				new_obj->setParent(this);
				return 1;
			}

			md::Type getType() override {
				return md::Type::Folder;
			}
	};

	class Group: public md::MeteoroidObject {
		private:
			std::vector<md::MeteoroidObject*> children;
		public:
			Group() {}
			Group(md::Folder* set_parent) : MeteoroidObject(set_parent) {}
			Group(md::Group* set_parent) : MeteoroidObject(set_parent) {}
			Group(std::string set_name) : MeteoroidObject(set_name) {}
			Group(md::Folder* set_parent, std::string set_name) : MeteoroidObject(set_parent, set_name) {}
			Group(md::Group* set_parent, std::string set_name) : MeteoroidObject(set_parent, set_name) {}

			int operator+=(md::MeteoroidObject* new_obj) {
				if(new_obj->getType() == md::Type::DNE || new_obj->getType() == md::Type::Folder) {
					return 0;
				}
				children.push_back(new_obj);
				new_obj->setParent(this);
				return 1;
			}

			md::Type getType() override {
				return md::Type::Group;
			}

	};

	class File: public md::MeteoroidObject {
		private:
			fs::path file_path;
		public:
			File() {}
			File(md::Folder* set_parent) : MeteoroidObject(set_parent) {}
			File(md::Group* set_parent) : MeteoroidObject(set_parent) {}
			File(std::string set_name) : MeteoroidObject(set_name) {}
			File(md::Folder* set_parent, std::string set_name) : MeteoroidObject(set_parent, set_name) {}
			File(md::Group* set_parent, std::string set_name) : MeteoroidObject(set_parent, set_name) {}

			md::Type getType() override {
				return md::Type::File;
			}

			File* setFilePath(fs::path set_file_path) {
				file_path = set_file_path;
				return this;
			}

	};

}



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
		row_data.push_back(col_data[col]);
		if((*out).max_column_width[col] < strlen(col_data[col])) {
			(*out).max_column_width[col] = strlen(col_data[col]);
		}
	}
	(*out).column_data.push_back(row_data);

	return 0;
}



int main() {

	md::Folder top_level_folder("main");
	top_level_folder.getParentType();


	sf::RenderWindow window(sf::VideoMode(1280, 720), "Meteoroid");
	window.setVerticalSyncEnabled(true);
	sf::View view = window.getDefaultView();

	sf::Clock clock;

	sqlite3* db;
	char* sql_err = 0;
	int sql_status;

	sql_status = sqlite3_open("test.db", &db);

	if( sql_status ) {

		printf("Can't open database: %s\n", sqlite3_errmsg(db));
		return 1;

	} else {

		printf("Opened database successfully\n");

	}


	sf::Font font;

	if(!font.loadFromFile("Assets/crystal.ttf")) {
		std::cout << "Error loading font" << std::endl;
	}

	std::string user_str = "";
	sf::Text user_text(user_str, font, 16);
	user_text.setFillColor(sf::Color::White);

	std::vector<sf::Text> text_archive;
	int selected_archive = -1;

	sf::RectangleShape carrot(sf::Vector2f(2,20));

	sf::Time time;

	float carrot_blink_timer = 0;
	uint64_t carrot_position = user_str.size();
	sql_table_s table_output;
	table_output.column_name.clear();
	table_output.column_data.clear();
	table_output.max_column_width.clear();

	uint8_t output_format = 2;
	bool path_updated = true;

	fs::path current_path = fs::current_path();
	std::vector<sf::Text> current_path_text;

	std::vector<sf::Text> available_files_text;
	std::vector<sf::Text> available_drives_text;

	while (window.isOpen()) {

		time = clock.restart();
		carrot_blink_timer += time.asSeconds() * 2;

		if(int(carrot_blink_timer) % 2) {
			carrot.setFillColor(sf::Color::White);
		} else {
			carrot.setFillColor(sf::Color::Transparent);
			if(carrot_blink_timer >= 2) {
				carrot_blink_timer = 0;
			}
		}

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {

				window.close();
				sqlite3_close(db);

			}

			if(event.type == sf::Event::MouseMoved) {
				for(auto & path_text : current_path_text) {
					if(path_text.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
						path_text.setOutlineThickness(1);
					} else if(path_text.getOutlineThickness() == 1) {
						path_text.setOutlineThickness(0);
					}
				}
				for(auto & file_text : available_files_text) {
					if(file_text.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
						file_text.setOutlineThickness(1);
					} else if(file_text.getOutlineThickness() == 1) {
						file_text.setOutlineThickness(0);
					}
				}
				for(auto & drive_text : available_drives_text) {
					if(drive_text.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
						drive_text.setOutlineThickness(1);
					} else if(drive_text.getOutlineThickness() == 1) {
						drive_text.setOutlineThickness(0);
					}
				}
			}

			if(event.type == sf::Event::MouseButtonPressed) {

				for(auto & path_text : current_path_text) {
					if(path_text.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {

						if(current_path_text.back().getString() == current_path.root_name().string()) {
							for(int i = 0; i < 26; i++) {
								std::string drive_letter = "";
								drive_letter += char('A' + i);
								drive_letter += ":";
								if( fs::exists(fs::path(drive_letter)) ) {
									available_drives_text.emplace_back(drive_letter, font, 16);
									available_drives_text.back().setPosition(0, 24*available_drives_text.size());
									available_drives_text.back().setOutlineColor(sf::Color::Blue);
								}
							}
							available_files_text.clear();
							break;
						}

						while(current_path_text.back().getString() != path_text.getString()) {
							current_path.remove_filename();
							current_path_text.pop_back();
						}
						path_updated = true;

					}
				}
				for(auto & file_text : available_files_text) {
					if(file_text.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
						current_path /= fs::path(std::string(file_text.getString()));
						path_updated = true;
					}
				}
				for(auto & drive_text : available_drives_text) {
					if(drive_text.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
						current_path = fs::path(std::string(drive_text.getString()) + "/");
						available_drives_text.clear();
						path_updated = true;
					}
				}

			}

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && carrot_position) {
				carrot_position--;
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && carrot_position < user_str.size()) {
				carrot_position++;
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && selected_archive+1 < text_archive.size()) {
				selected_archive++;
				user_str = text_archive[selected_archive].getString();
				carrot_position = user_str.size();
				user_text.setString(user_str);
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && selected_archive >= 0) {
				selected_archive--;
				if(selected_archive == -1) {
					user_str = "";
				} else {
					user_str = text_archive[selected_archive].getString();
				}
				carrot_position = user_str.size();
				user_text.setString(user_str);
			}

			if (event.type == sf::Event::TextEntered) {
				uint8_t user_input = event.text.unicode;
				if (31 < user_input && user_input < 128) {

					user_str.insert(user_str.begin() + carrot_position, static_cast<char>(user_input));
					carrot_position++;
					carrot_blink_timer = 1;

				} else if(user_input == 8 && user_str.size()) {

					user_str.erase(carrot_position-1, 1);
					carrot_position--;
					carrot_blink_timer = 1;

				} else if(user_input == 13) {

					if(user_str.substr(0,5) == "SQL: ") {

						table_output.column_name.clear();
						table_output.column_data.clear();
						table_output.max_column_width.clear();

						sql_status = sqlite3_exec(
							db,
							user_str.substr(5).c_str(),
							callback,
							&table_output,
							&sql_err);
						if( sql_status!=SQLITE_OK ) {
							printf("SQL error: %s\n", sql_err);
							sqlite3_free(sql_err);
						}

						output_format = 1;

					} else {
						output_format = 0;
					}

					user_str = "";
					selected_archive = -1;
					carrot_position = 0;
					carrot_blink_timer = 1;
					text_archive.insert(text_archive.begin(), user_text);
					user_text.move(0,20);


				}
				user_text.setString(user_str);
			}

			if(event.type = sf::Event::MouseWheelScrolled) {

				view.move(0, -event.mouseWheelScroll.delta * 24);

			}

		}

		if(output_format == 2 && path_updated) {

			current_path_text.clear();
			for (auto & current_path_element : current_path) {

				if(current_path_element == "\\") continue;

				if(current_path_text.empty()) {
					current_path_text.emplace_back(current_path_element.string(), font, 16);
					current_path_text.back().setPosition(sf::Vector2f(0, 0));
				} else {
					current_path_text.emplace_back("/" + current_path_element.string(), font, 16);
					current_path_text.back().setPosition(current_path_text[current_path_text.size() - 2].findCharacterPos(-1));
				}

				current_path_text.back().setOutlineColor(sf::Color::Blue);

			}

			available_files_text.clear();
			int dir_pos = 0;
			for (auto & curr_file : fs::directory_iterator(current_path)) {

				switch(curr_file.status().type()) {
					case fs::file_type::regular:
						available_files_text.emplace_back(curr_file.path().filename().string(), font, 16);
						available_files_text.back().setFillColor(sf::Color(0xAA, 0xFF, 0xAA));
						available_files_text.back().setOutlineColor(sf::Color(0x00, 0xAA, 0x00));
						break;
					case fs::file_type::directory:
						available_files_text.emplace(available_files_text.begin()+dir_pos, curr_file.path().filename().string(), font, 16);
						available_files_text[dir_pos].setFillColor(sf::Color(0xAA, 0xAA, 0xFF));
						available_files_text[dir_pos].setOutlineColor(sf::Color(0x00, 0x00, 0xAA));
						dir_pos++;
						break;
					default:
						available_files_text.emplace_back(curr_file.path().filename().string(), font, 16);
						available_files_text.back().setFillColor(sf::Color(0xFF, 0xAA, 0xAA));
						available_files_text.back().setOutlineColor(sf::Color(0xAA, 0x00, 0x00));
						break;
				}

			}
			for (int i = 0; i < available_files_text.size(); i++) {

				available_files_text[i].setPosition(0, 24+ 24*i);

			}

			path_updated = false;

		}

		carrot.setPosition(user_text.findCharacterPos(carrot_position));

		window.clear();
		if(output_format == 0) {

			for(int i = 0; i < text_archive.size(); i++) {
				window.draw(text_archive[i]);
			}

			window.draw(user_text);
			window.draw(carrot);

		} else if(output_format == 1) {

			size_t total_row_width = 0;
			for(int col = 0; col < table_output.column_name.size(); col++) {
				sf::RectangleShape table_cell(sf::Vector2f(table_output.max_column_width[col]*8 + 8,24));
				table_cell.setPosition(sf::Vector2f(640 + total_row_width, 10));
				table_cell.setFillColor(sf::Color::Black);
				table_cell.setOutlineThickness(1);
				table_cell.setOutlineColor(sf::Color::White);
				window.draw(table_cell);
				sf::Text cell_text(table_output.column_name[col], font, 16);
				cell_text.setPosition(640 + total_row_width + 4, 12);
				window.draw(cell_text);
				total_row_width += table_output.max_column_width[col]*8 + 8;
			}
			for(int row = 0; row < table_output.column_data.size(); row++) {
				total_row_width = 0;
				for(int col = 0; col < table_output.column_name.size(); col++) {
					sf::RectangleShape table_cell(sf::Vector2f(table_output.max_column_width[col]*8 + 8,24));
					table_cell.setPosition(sf::Vector2f(640 + total_row_width, 34 + 24*row));
					table_cell.setFillColor(sf::Color::Black);
					table_cell.setOutlineThickness(1);
					table_cell.setOutlineColor(sf::Color::White);
					window.draw(table_cell);
					sf::Text cell_text(table_output.column_data[row][col], font, 16);
					cell_text.setPosition(640 + total_row_width + 4, 34 + 24*row + 2);
					window.draw(cell_text);
					total_row_width += table_output.max_column_width[col]*8 + 8;
				}
			}

		} else if(output_format == 2) {

			for (auto path_text : current_path_text) {
				window.draw(path_text);
			}
			for (auto file_text : available_files_text) {
				window.draw(file_text);
			}
			for (auto drive_text : available_drives_text) {
				window.draw(drive_text);
			}


		}

		window.setView(view);
		window.display();

	}

	return 0;

}
