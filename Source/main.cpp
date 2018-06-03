#include <iostream>
#include <vector>
#include <string>

#include <stdio.h>
#include <math.h>

#include <sqlite3.h>

#include <SFML/Graphics.hpp>

using namespace std;

struct sql_table_s {
	vector<string> column_name;
	vector<vector<string>> column_data;
	vector<size_t> max_column_width;
};

static int callback(void *table_output, int col_count, char **col_data, char **col_name) {

	sql_table_s *out = static_cast<sql_table_s*>(table_output);

	if( !(*out).column_name.size() ) {
		for(int col = 0; col < col_count; col++) {
			(*out).column_name.push_back(col_name[col]);
			(*out).max_column_width.push_back(strlen(col_name[col]));
		}
	}

	vector<string> row_data;
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

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Meteoroid");
	window.setVerticalSyncEnabled(true);

	sf::Clock clock;

	sqlite3 *db;
	char *sql_err = 0;
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
		cout << "Error loading font" << endl;
	}

	sf::Text user_text;
	sf::String user_string = "";
	user_text.setFont(font);
	user_text.setString(user_string);
	user_text.setCharacterSize(16);
	user_text.setFillColor(sf::Color::White);

	vector<sf::Text> text_archive;
	int selected_archive = -1;

	sf::RectangleShape carrot(sf::Vector2f(2,20));

	sf::Time time;

	float carrot_blink_timer = 0;
	uint64_t carrot_position = user_string.getSize();
	sql_table_s table_output;
	table_output.column_name.clear();
	table_output.column_data.clear();
	table_output.max_column_width.clear();

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

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && carrot_position) {
				carrot_position--;
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && carrot_position < user_string.getSize()) {
				carrot_position++;
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && selected_archive+1 < text_archive.size()) {
				selected_archive++;
				user_string = text_archive[selected_archive].getString();
				carrot_position = user_string.getSize();
				user_text.setString(user_string);
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && selected_archive >= 0) {
				selected_archive--;
				if(selected_archive == -1) {
					user_string = "";
				} else {
					user_string = text_archive[selected_archive].getString();
				}
				carrot_position = user_string.getSize();
				user_text.setString(user_string);
			}

			if (event.type == sf::Event::TextEntered) {
				uint8_t user_input = event.text.unicode;
				if (31 < user_input && user_input < 128) {

					user_string.insert(carrot_position, static_cast<char>(user_input));
					carrot_position++;
					carrot_blink_timer = 1;

				} else if(user_input == 8 && user_string.getSize()) {

					user_string.erase(carrot_position-1, 1);
					carrot_position--;
					carrot_blink_timer = 1;

				} else if(user_input == 13) {

					if(user_string.substring(0,5) == "SQL: ") {

						table_output.column_name.clear();
						table_output.column_data.clear();
						table_output.max_column_width.clear();

						sql_status = sqlite3_exec(
							db,
							user_string.substring(5).toAnsiString().c_str(),
							callback,
							&table_output,
							&sql_err);
						if( sql_status!=SQLITE_OK ) {
							printf("SQL error: %s\n", sql_err);
							sqlite3_free(sql_err);
						}

					}

					user_string = "";
					selected_archive = -1;
					carrot_position = 0;
					carrot_blink_timer = 1;
					text_archive.insert(text_archive.begin(), user_text);
					user_text.move(0,20);


				}
				user_text.setString(user_string);
			}

		}

		carrot.setPosition(user_text.findCharacterPos(carrot_position));

		window.clear();

		for(int i = 0; i < text_archive.size(); i++) {
			window.draw(text_archive[i]);
		}

		window.draw(user_text);
		window.draw(carrot);

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

		window.display();

	}

	return 0;

}
