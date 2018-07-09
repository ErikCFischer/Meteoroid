//C++ Standard Library
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
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
#include <md/Playlist.hpp>
#include <md/File.hpp>

#include <EventThread.hpp>


#define dbp(x) std::cout << x << std::endl;


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

void loadMDFromDB(sqlite3*, md::MeteorItem*, int);

bool hovered(sf::Text&, sf::RenderWindow&);
bool hovered(sf::Shape&, sf::RenderWindow&);
bool hovered(sf::Text&, sf::RenderWindow&, sf::View&);
bool hovered(sf::Shape&, sf::RenderWindow&, sf::View&);

bool hovered(sf::View&, sf::RenderWindow&);

int main() {

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Meteoroid");
	sf::View total_window_view(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
	
	sf::RenderTexture render_md;
	render_md.create(window.getSize().x, window.getSize().y);
	sf::View view_md(sf::FloatRect(0, 0, window.getSize().x/2, window.getSize().y));
	view_md.setViewport(sf::FloatRect(0, 0, 0.5, 1));

	sf::RenderTexture render_fs;
	render_fs.create(window.getSize().x, window.getSize().y);
	sf::View view_fs(sf::FloatRect(0, 0, window.getSize().x/2, window.getSize().y));
	view_fs.setViewport(sf::FloatRect(0.5, 0, 0.5, 1));

	sf::Clock clock;

	sf::Font font;

	if(!font.loadFromFile("Assets/crystal.ttf")) {
		dbp("Error loading font");
	}

	sf::Color folder_fill_c(0xA0, 0xA0, 0xF0);
	sf::Color folder_outline_c(0x40, 0x40, 0x60);

	sf::Color playlist_fill_c(0xA0, 0xF0, 0xA0);
	sf::Color playlist_outline_c(0x40, 0x60, 0x40);

	sf::Color file_fill_c(0xF0, 0xA0, 0xA0);
	sf::Color file_outline_c(0x60, 0x40, 0x40);

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

	uint8_t output_format = 4;
	uint8_t context_menu = 0;
	sf::Vector2f context_menu_pos;

	bool path_updated = true;

	fs::path current_path_fs = fs::current_path();
	std::vector<sf::Text> path_fs_txt_vctr;

	std::vector<sf::Text> item_fs_txt_vctr;
	std::vector<sf::Text> root_fs_txt_vctr;

	sqlite3* db;
	char* sql_err = 0;
	int sql_status;

	sql_status = sqlite3_open("test.db", &db);

	if( sql_status != SQLITE_OK ) {

		dbp("Can't open database: " << sqlite3_errmsg(db));
		return 1;

	} else {

		dbp("Opened database successfully");

	}

	char* sql_statement;
	//sql_statement = "CREATE TABLE md_item(ID INT NOT NULL, name TEXT NOT NULL, parent_ID INT, type CHAR(8));";
	/*
	sql_statement = "INSERT INTO md_item VALUES(0, '/', NULL, 'Folder');" \
					"INSERT INTO md_item VALUES(1, 'Media', 0, 'Folder');" \
					"INSERT INTO md_item VALUES(2, 'TV', 1, 'Folder');" \
					"INSERT INTO md_item VALUES(3, 'Metalocalypse', 2, 'Playlist');" \
					"INSERT INTO md_item VALUES(4, 'Season 1', 3, 'Playlist');" \
					"INSERT INTO md_item VALUES(5, 'Episode 1', 4, 'File');" \
					"INSERT INTO md_item VALUES(6, 'Episode 2', 4, 'File');" \
					"INSERT INTO md_item VALUES(7, 'Episode 3', 4, 'File');" \
					"INSERT INTO md_item VALUES(8, 'Episode 4', 4, 'File');" \
					"INSERT INTO md_item VALUES(9, 'Episode 5', 4, 'File');" \
					"INSERT INTO md_item VALUES(10, 'Episode 6', 4, 'File');" \
					"INSERT INTO md_item VALUES(11, 'Episode 7', 4, 'File');" \
					"INSERT INTO md_item VALUES(12, 'Episode 8', 4, 'File');" \
					"INSERT INTO md_item VALUES(13, 'Season 2', 3, 'Playlist');" \
					"INSERT INTO md_item VALUES(14, 'Episode 1', 13, 'File');" \
					"INSERT INTO md_item VALUES(15, 'Episode 2', 13, 'File');" \
					"INSERT INTO md_item VALUES(16, 'Episode 3', 13, 'File');" \
					"INSERT INTO md_item VALUES(17, 'Episode 4', 13, 'File');" \
					"INSERT INTO md_item VALUES(18, 'Episode 5', 13, 'File');" \
					"INSERT INTO md_item VALUES(19, 'Episode 6', 13, 'File');" \
					"INSERT INTO md_item VALUES(20, 'Episode 7', 13, 'File');" \
					"INSERT INTO md_item VALUES(21, 'Episode 8', 13, 'File');" \
					"INSERT INTO md_item VALUES(22, 'Movies', 1, 'Folder');" \
					"INSERT INTO md_item VALUES(23, 'Blade', 22, 'Playlist');" \
					"INSERT INTO md_item VALUES(24, 'Blade 1', 23, 'File');" \
					"INSERT INTO md_item VALUES(25, 'Blade 2', 23, 'File');" \
					"INSERT INTO md_item VALUES(26, 'Blade 3', 23, 'File');" \
					"INSERT INTO md_item VALUES(27, 'Django', 22, 'File');" \
					"INSERT INTO md_item VALUES(28, 'Planet Terror', 22, 'File');" \
					"INSERT INTO md_item VALUES(29, 'Anime', 1, 'Folder');" \
					"INSERT INTO md_item VALUES(30, 'Games', 0, 'Folder');";
	*/

	sql_statement = "SELECT * FROM md_item WHERE ID = 0;";

	sql_table_s md_table;

	/* Execute SQL statement */
	sql_status = sqlite3_exec(db, sql_statement, callback, &md_table, &sql_err);

	if( sql_status != SQLITE_OK ){
		dbp("SQL error: " << sql_err);
		sqlite3_free(sql_err);
	} else {
		dbp("Loaded Virtual Root");
	}

	md::MeteorItem* current_item_md = new md::Folder(md_table.column_data[0][1]);

	loadMDFromDB(db, current_item_md, 0);

	dbp("MD Fully Loaded");

	std::vector<sf::Text> path_md_txt_vctr;
	std::vector<sf::Text> item_md_txt_vctr;

	std::mutex mute;
	std::condition_variable cv;
	thread_info_s thread_info = {false, "", sf::Vector2i(0,0)};

	std::thread new_event_thread(eventThread, std::ref(thread_info), std::ref(mute), std::ref(cv));

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

			if(event.type == sf::Event::KeyReleased) {
				if(event.key.code == sf::Keyboard::BackSpace) {
					cv.notify_all();
				}
			}

			if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				
				std::unique_lock<std::mutex> lck(mute);
				thread_info.kill = true;
				cv.notify_all();

				window.close();
				sqlite3_close(db);
			}

			if (event.type == sf::Event::Resized) {
				
				total_window_view = sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height));

				view_md.reset(sf::FloatRect(0, 0, event.size.width * view_md.getViewport().width, event.size.height));
				view_fs.reset(sf::FloatRect(0, 0, event.size.width * view_fs.getViewport().width, event.size.height));

			}

			if(event.type == sf::Event::MouseMoved) {
				
				for(auto & txt : path_fs_txt_vctr) {
					if(hovered(txt, window, view_fs)) {
						txt.setOutlineThickness(1);
					} else if(txt.getOutlineThickness() == 1) {
						txt.setOutlineThickness(0);
					}
				}
				for(auto & txt : item_fs_txt_vctr) {
					if(hovered(txt, window, view_fs)) {
						txt.setOutlineThickness(1);
					} else if(txt.getOutlineThickness() == 1) {
						txt.setOutlineThickness(0);
					}
				}
				for(auto & txt : root_fs_txt_vctr) {
					if(hovered(txt, window, view_fs)) {
						txt.setOutlineThickness(1);
					} else if(txt.getOutlineThickness() == 1) {
						txt.setOutlineThickness(0);
					}
				}
				
				for(auto & txt : path_md_txt_vctr) {
					if(hovered(txt, window, view_md)) {
						txt.setOutlineThickness(1);
					} else if(txt.getOutlineThickness() == 1) {
						txt.setOutlineThickness(0);
					}
				}
				for(auto & txt : item_md_txt_vctr) {
					if(hovered(txt, window, view_md)) {
						txt.setOutlineThickness(1);
					} else if(txt.getOutlineThickness() == 1) {
						txt.setOutlineThickness(0);
					}
				}
			}

			if(event.type == sf::Event::MouseButtonPressed) {
				if(event.mouseButton.button == sf::Mouse::Left) {

					context_menu = 0;

					for(auto & path_fs_seg_txt : path_fs_txt_vctr) {
						if(hovered(path_fs_seg_txt, window, view_fs)) {

							if(path_fs_txt_vctr.back().getString() == current_path_fs.root_name().string()) {
								for(int i = 0; i < 26; i++) {
									std::string drive_letter = "";
									drive_letter += char('A' + i);
									drive_letter += ":";
									if( fs::exists(fs::path(drive_letter)) ) {
										root_fs_txt_vctr.emplace_back(drive_letter, font, 16);
										root_fs_txt_vctr.back().setPosition(0, 24*root_fs_txt_vctr.size());
										root_fs_txt_vctr.back().setOutlineColor(sf::Color::Blue);
									}
								}
								item_fs_txt_vctr.clear();
								break;
							}

							while(path_fs_txt_vctr.back().getString() != path_fs_seg_txt.getString()) {
								current_path_fs.remove_filename();
								path_fs_txt_vctr.pop_back();
							}
							path_updated = true;

						}
					}
					for(auto & item_fs_txt : item_fs_txt_vctr) {
						if(hovered(item_fs_txt, window, view_fs)) {
							current_path_fs /= fs::path(std::string(item_fs_txt.getString()));
							path_updated = true;
						}
					}
					for(auto & root_fs_txt : root_fs_txt_vctr) {
						if(hovered(root_fs_txt, window, view_fs)) {
							current_path_fs = fs::path(std::string(root_fs_txt.getString()) + "/");
							root_fs_txt_vctr.clear();
							path_updated = true;
						}
					}
					for(auto & path_md_seg_txt : path_md_txt_vctr) {
						if(hovered(path_md_seg_txt, window, view_md)) {
							while(path_md_txt_vctr.back().getString() != path_md_seg_txt.getString()) {
								current_item_md = current_item_md->getParent();
								path_md_txt_vctr.pop_back();
							}
							path_updated = true;
						}
					}
					for(auto & item_md_txt : item_md_txt_vctr) {
						if(hovered(item_md_txt, window, view_md)) {
							if(md::Container* current_container = dynamic_cast<md::Container*>(current_item_md)) {
								current_item_md = current_container->at(int(item_md_txt.getPosition().y) / 24 - 1);
							}
							path_updated = true;
						}
					}
				} else if (event.mouseButton.button == sf::Mouse::Right) {
					context_menu = 1;
					context_menu_pos = window.mapPixelToCoords(sf::Mouse::getPosition(window)) + sf::Vector2f(1,3);
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
				if(event.mouseWheelScroll.delta == 1) {
					if(hovered(view_md, window) && view_md.getCenter().y - view_md.getSize().y/2 > 0) {
						view_md.move(0, -24);
					}
					if(hovered(view_fs, window) && view_fs.getCenter().y - view_fs.getSize().y/2 > 0) {
						view_fs.move(0, -24);
					}
				} else if (event.mouseWheelScroll.delta == -1) {
					if(hovered(view_md, window) && view_md.getCenter().y + view_md.getSize().y/2 < render_md.getSize().y) {
						view_md.move(0, 24);
					}
					if(hovered(view_fs, window) && view_fs.getCenter().y + view_fs.getSize().y/2 < render_fs.getSize().y) {
						view_fs.move(0, 24);
					}
				}
			}

		}

		if((output_format == 2 || output_format == 4) && path_updated) {

			int max_fs_width = 0;
			int max_fs_height = 0;

			path_fs_txt_vctr.clear();
			
			for (auto & path_fs_seg : current_path_fs) {

				if(path_fs_seg == "\\") continue;

				if(path_fs_txt_vctr.empty()) {
					path_fs_txt_vctr.emplace_back(path_fs_seg.string(), font, 16);
					path_fs_txt_vctr.back().setPosition(sf::Vector2f(0, 0));
				} else {
					path_fs_txt_vctr.emplace_back("/" + path_fs_seg.string(), font, 16);
					path_fs_txt_vctr.back().setPosition(path_fs_txt_vctr[path_fs_txt_vctr.size() - 2].findCharacterPos(-1));
				}

				path_fs_txt_vctr.back().setOutlineColor(sf::Color::Blue);

			}
			
			max_fs_width = path_fs_txt_vctr.back().findCharacterPos(-1).x;
			max_fs_height = 24;

			item_fs_txt_vctr.clear();
			int dir_pos = 0;
			for (auto & current_item_fs : fs::directory_iterator(current_path_fs)) {

				switch(current_item_fs.status().type()) {
					case fs::file_type::regular:
						item_fs_txt_vctr.emplace_back(current_item_fs.path().filename().string(), font, 16);
						item_fs_txt_vctr.back().setFillColor(sf::Color(0xAA, 0xFF, 0xAA));
						item_fs_txt_vctr.back().setOutlineColor(sf::Color(0x00, 0xAA, 0x00));
						break;
					case fs::file_type::directory:
						item_fs_txt_vctr.emplace(item_fs_txt_vctr.begin()+dir_pos, current_item_fs.path().filename().string(), font, 16);
						item_fs_txt_vctr[dir_pos].setFillColor(sf::Color(0xAA, 0xAA, 0xFF));
						item_fs_txt_vctr[dir_pos].setOutlineColor(sf::Color(0x00, 0x00, 0xAA));
						dir_pos++;
						break;
					default:
						item_fs_txt_vctr.emplace_back(current_item_fs.path().filename().string(), font, 16);
						item_fs_txt_vctr.back().setFillColor(sf::Color(0xFF, 0xAA, 0xAA));
						item_fs_txt_vctr.back().setOutlineColor(sf::Color(0xAA, 0x00, 0x00));
						break;
				}

			}

			for (int i = 0; i < item_fs_txt_vctr.size(); i++) {

				item_fs_txt_vctr[i].setPosition(0, 24 + 24*i);

				if(max_fs_width < item_fs_txt_vctr[i].findCharacterPos(-1).x) {
					max_fs_width = item_fs_txt_vctr[i].findCharacterPos(-1).x;
				}
				max_fs_height += 24;

			}

			render_fs.create(max_fs_width, max_fs_height);
			if(output_format == 3)
				path_updated = false;

		}
		if((output_format == 3 || output_format == 4) && path_updated) {

			int max_md_width = 0;
			int max_md_height = 0;

			path_md_txt_vctr.clear();

			md::MeteorItem* path_md_seg = current_item_md;
			while(path_md_seg) {

				path_md_txt_vctr.emplace(path_md_txt_vctr.begin(), "/" + path_md_seg->name(), font, 16);
				
				if(path_md_seg->getType() == md::Type::Folder) {
					path_md_txt_vctr[0].setFillColor(folder_fill_c);
					path_md_txt_vctr[0].setOutlineColor(folder_outline_c);
				} else if(path_md_seg->getType() == md::Type::Playlist) {
					path_md_txt_vctr[0].setFillColor(playlist_fill_c);
					path_md_txt_vctr[0].setOutlineColor(playlist_outline_c);
				} else if(path_md_seg->getType() == md::Type::File) {
					path_md_txt_vctr[0].setFillColor(file_fill_c);
					path_md_txt_vctr[0].setOutlineColor(file_outline_c);
				}
				path_md_seg = path_md_seg->getParent();

			}

			for(int i = 1; i < path_md_txt_vctr.size(); i++) {
				path_md_txt_vctr[i].setPosition(path_md_txt_vctr[i - 1].findCharacterPos(-1));
			}

			max_md_width = path_md_txt_vctr.back().findCharacterPos(-1).x;
			max_md_height = 24;


			item_md_txt_vctr.clear();
			if(md::Container* current_container = dynamic_cast<md::Container*>(current_item_md)) {
				for (int i = 0; i < current_container->size(); i++) {

					item_md_txt_vctr.emplace_back(current_container->at(i)->name(), font, 16);
					item_md_txt_vctr.back().setPosition(sf::Vector2f(0, 24 + 24*i));

					if(current_container->at(i)->getType() == md::Type::Folder) {
						item_md_txt_vctr.back().setFillColor(folder_fill_c);
						item_md_txt_vctr.back().setOutlineColor(folder_outline_c);
					} else if(current_container->at(i)->getType() == md::Type::Playlist) {
						item_md_txt_vctr.back().setFillColor(playlist_fill_c);
						item_md_txt_vctr.back().setOutlineColor(playlist_outline_c);
					} else if(current_container->at(i)->getType() == md::Type::File) {
						item_md_txt_vctr.back().setFillColor(file_fill_c);
						item_md_txt_vctr.back().setOutlineColor(file_outline_c);
					}

					if(max_md_width < item_md_txt_vctr.back().findCharacterPos(-1).x) {
						max_md_width = item_md_txt_vctr.back().findCharacterPos(-1).x;
					}
					max_md_height += 24;

				}
			} else {
				std::cout << "File" << std::endl;
			}

			render_md.create(max_md_width, max_md_height);
			path_updated = false;

		}

		carrot.setPosition(user_text.findCharacterPos(carrot_position));

		window.clear();
		render_fs.clear();
		render_md.clear();

		if(output_format == 0) {

			for(int i = 0; i < text_archive.size(); i++) {
				window.draw(text_archive[i]);
			}

			window.draw(user_text);
			window.draw(carrot);

		} 
		if(output_format == 1) {

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

		}
		if(output_format == 2 || output_format == 4) {

			for (auto path_text : path_fs_txt_vctr) {
				render_fs.draw(path_text);
			}
			for (auto file_text : item_fs_txt_vctr) {
				render_fs.draw(file_text);
			}
			for (auto drive_text : root_fs_txt_vctr) {
				render_fs.draw(drive_text);
			}
			

			render_fs.display();
			const sf::Texture& texture_fs = render_fs.getTexture();
			sf::Sprite sprite_fs(texture_fs);
			window.setView(view_fs);
			window.draw(sprite_fs);


		}
		if(output_format == 3 || output_format == 4) {
			
			for (auto path_text : path_md_txt_vctr) {
				render_md.draw(path_text);
			}

			for (auto item_text : item_md_txt_vctr) {
				render_md.draw(item_text);
			}

			render_md.display();

			sf::Texture texture_md = render_md.getTexture();
			sf::Sprite sprite_md(texture_md);
			window.setView(view_md);
			window.draw(sprite_md);

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

			menu_item_text.emplace_back("New Playlist", font, 16);
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
				if(hovered(menu_item.back(), window)) {
					menu_item.back().setFillColor(sf::Color(0xA0, 0xA0, 0xFF));
				}
				menu_item.back().setOutlineColor(sf::Color(0x80, 0x80, 0x80));
				menu_item.back().setOutlineThickness(1);
			}

			window.setView(total_window_view);

			for(auto & item : menu_item)
				window.draw(item);
			for(auto & item_text : menu_item_text)
				window.draw(item_text);

		}

		window.display();

	}

	new_event_thread.join();

	return 0;

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

bool hovered(sf::Text& txt, sf::RenderWindow& window) {
	return (txt.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) ? true : false;
}
bool hovered(sf::Shape& shape, sf::RenderWindow& window) {
	return (shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) ? true : false;
}

bool hovered(sf::Text& txt, sf::RenderWindow& window, sf::View& view) {
	return (txt.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view))) ? true : false;
}
bool hovered(sf::Shape& shape, sf::RenderWindow& window, sf::View& view) {
	return (shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view))) ? true : false;
}

bool hovered(sf::View& view, sf::RenderWindow& window) {
	sf::FloatRect view_rect;
	view_rect.width = view.getSize().x;
	view_rect.height = view.getSize().y;
	view_rect.left = view.getCenter().x - view.getSize().x/2;
	view_rect.top = view.getCenter().y - view.getSize().y/2;
	return (view_rect.contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view))) ? true : false;
}