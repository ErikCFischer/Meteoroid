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
//Local includes
#include <md/Folder.hpp>
#include <md/Group.hpp>
#include <md/File.hpp>

//File system library
namespace fs = std::experimental::filesystem;

//Struct to hold an SQL table
struct sql_table_s {
	std::vector<std::string> column_name;
	std::vector<std::vector<std::string>> column_data;
	std::vector<size_t> max_column_width;
};

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

	md::Folder root("root");
		md::Folder media("Media", &root);
			md::Folder tv("TV", &media);
				md::Group metal("Metalocalypse", &tv);
					md::Group metal_s1("Season 1", &metal);
						md::File metal_s1e1("Episode 1", &metal_s1);
						md::File metal_s1e2("Episode 2", &metal_s1);
						md::File metal_s1e3("Episode 3", &metal_s1);
						md::File metal_s1e4("Episode 4", &metal_s1);
						md::File metal_s1e5("Episode 5", &metal_s1);
					md::Group metal_s2("Season 2", &metal);
						md::File metal_s2e1("Episode 1", &metal_s2);
						md::File metal_s2e2("Episode 2", &metal_s2);
						md::File metal_s2e3("Episode 3", &metal_s2);
						md::File metal_s2e4("Episode 4", &metal_s2);
						md::File metal_s2e5("Episode 5", &metal_s2);
			md::Folder movies("Movies", &media);
				md::Group blade("Blade", &movies);
					md::File blade1("Blade 1", &blade);
					md::File blade2("Blade 2", &blade);
					md::File blade3("Blade 3", &blade);
				md::File django("Django", &movies);
				md::File planet("Planet Terror", &movies);
			md::Folder anime("Anime", &media);
		md::Folder games("Games", &root);

	md::MeteoroidObject* current_item = &root;
	std::vector<sf::Text> virtual_path_text;
	std::vector<sf::Text> virtual_item_text;

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

	uint8_t output_format = 3;
	uint8_t context_menu = 0;
	sf::Vector2f context_menu_pos;

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
				for(auto & path_text : virtual_path_text) {
					if(path_text.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
						path_text.setOutlineThickness(1);
					} else if(path_text.getOutlineThickness() == 1) {
						path_text.setOutlineThickness(0);
					}
				}
				for(auto & item_text : virtual_item_text) {
					if(item_text.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
						item_text.setOutlineThickness(1);
					} else if(item_text.getOutlineThickness() == 1) {
						item_text.setOutlineThickness(0);
					}
				}
			}

			if(event.type == sf::Event::MouseButtonPressed) {
				if(event.mouseButton.button == sf::Mouse::Left) {
					context_menu = 0;
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
					for(auto & path_text : virtual_path_text) {
						if(path_text.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
							while(virtual_path_text.back().getString() != path_text.getString()) {
								current_item = current_item->getParent();
								virtual_path_text.pop_back();
							}
							path_updated = true;
						}
					}
					for(auto & item_text : virtual_item_text) {
						if(item_text.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
							if(md::Container* current_container = dynamic_cast<md::Container*>(current_item)) {
								current_item = current_container->at(int(item_text.getPosition().y) / 24 - 1);
							}
							path_updated = true;
						}
					}
				} else if (event.mouseButton.button == sf::Mouse::Right) {
					context_menu = 1;
					context_menu_pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
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

				if(event.mouseWheelScroll.delta == 1 || event.mouseWheelScroll.delta == -1)
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

				available_files_text[i].setPosition(0, 24 + 24*i);

			}

			path_updated = false;

		}

		if(output_format == 3 && path_updated) {

			virtual_path_text.clear();
			md::MeteoroidObject* temp_curr_item = current_item;
			while(temp_curr_item) {
				virtual_path_text.emplace(virtual_path_text.begin(), "/" + temp_curr_item->name(), font, 16);
				
				if(temp_curr_item->getType() == md::Type::Folder) {
					virtual_path_text[0].setFillColor(sf::Color(0xAA, 0xFF, 0xAA));
					virtual_path_text[0].setOutlineColor(sf::Color(0x00, 0xAA, 0x00));
				} else if(temp_curr_item->getType() == md::Type::Group) {
					virtual_path_text[0].setFillColor(sf::Color(0xAA, 0xAA, 0xFF));
					virtual_path_text[0].setOutlineColor(sf::Color(0x00, 0x00, 0xAA));
				} else if(temp_curr_item->getType() == md::Type::File) {
					virtual_path_text[0].setFillColor(sf::Color(0xFF, 0xAA, 0xAA));
					virtual_path_text[0].setOutlineColor(sf::Color(0xAA, 0x00, 0x00));
				}
				temp_curr_item = temp_curr_item->getParent();
			}
			for(int i = 1; i < virtual_path_text.size(); i++) {
				virtual_path_text[i].setPosition(virtual_path_text[i - 1].findCharacterPos(-1));
			}


			virtual_item_text.clear();
			if(md::Container* current_container = dynamic_cast<md::Container*>(current_item)) {
				for (int i = 0; i < current_container->size(); i++) {

					virtual_item_text.emplace_back(current_container->at(i)->name(), font, 16);
					virtual_item_text.back().setPosition(sf::Vector2f(0, 24 + 24*i));

					if(current_container->at(i)->getType() == md::Type::Folder) {
						virtual_item_text.back().setFillColor(sf::Color(0xAA, 0xFF, 0xAA));
						virtual_item_text.back().setOutlineColor(sf::Color(0x00, 0xAA, 0x00));
					} else if(current_container->at(i)->getType() == md::Type::Group) {
						virtual_item_text.back().setFillColor(sf::Color(0xAA, 0xAA, 0xFF));
						virtual_item_text.back().setOutlineColor(sf::Color(0x00, 0x00, 0xAA));
					} else if(current_container->at(i)->getType() == md::Type::File) {
						virtual_item_text.back().setFillColor(sf::Color(0xFF, 0xAA, 0xAA));
						virtual_item_text.back().setOutlineColor(sf::Color(0xAA, 0x00, 0x00));
					}

				}
			} else {
				std::cout << "File" << std::endl;
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


		} else if(output_format == 3) {

			for (auto path_text : virtual_path_text) {
				window.draw(path_text);
			}

			for (auto item_text : virtual_item_text) {
				window.draw(item_text);
			}

		}

		if(context_menu == 1) {

			std::vector<sf::Text> menu_item_text;
			std::vector<sf::RectangleShape> menu_item;
			size_t largest_width = 0;

			menu_item_text.emplace_back("New Folder", font, 16);
			menu_item_text.back().setPosition(sf::Vector2f(context_menu_pos.x + 4, context_menu_pos.y + 24*0 + 2));
			menu_item_text.back().setFillColor(sf::Color(0x00, 0x00, 0x00));
			if(largest_width < menu_item_text.back().getGlobalBounds().width + 8)
				largest_width = menu_item_text.back().getGlobalBounds().width + 8;

			menu_item_text.emplace_back("New Group", font, 16);
			menu_item_text.back().setPosition(sf::Vector2f(context_menu_pos.x + 4, context_menu_pos.y + 24*1 + 2));
			menu_item_text.back().setFillColor(sf::Color(0x00, 0x00, 0x00));
			if(largest_width < menu_item_text.back().getGlobalBounds().width + 8)
				largest_width = menu_item_text.back().getGlobalBounds().width + 8;

			menu_item_text.emplace_back("Add File", font, 16);
			menu_item_text.back().setPosition(sf::Vector2f(context_menu_pos.x + 4, context_menu_pos.y + 24*2 + 2));
			menu_item_text.back().setFillColor(sf::Color(0x00, 0x00, 0x00));
			if(largest_width < menu_item_text.back().getGlobalBounds().width + 8)
				largest_width = menu_item_text.back().getGlobalBounds().width + 8;


			for(int i = 0; i < menu_item_text.size(); i++) {
				menu_item.emplace_back(sf::Vector2f(largest_width, 24));
				menu_item.back().setPosition(sf::Vector2f(context_menu_pos.x, context_menu_pos.y + 24*i));
				menu_item.back().setFillColor(sf::Color(0xC0, 0xC0, 0xC0));
				if(menu_item.back().getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
					menu_item.back().setFillColor(sf::Color(0xA0, 0xA0, 0xFF));
				}
				menu_item.back().setOutlineColor(sf::Color(0x80, 0x80, 0x80));
				menu_item.back().setOutlineThickness(1);
			}

			for(auto & item : menu_item)
				window.draw(item);
			for(auto & item_text : menu_item_text)
				window.draw(item_text);



		}

		window.setView(view);
		window.display();

	}

	return 0;

}
