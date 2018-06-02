#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <stdio.h>
#include <sqlite3.h>

#include <SFML/Graphics.hpp>

using namespace std;

struct table_output {
	vector<string> column_name;
	vector<vector<string>> column_data;
	vector<size_t> max_column_width;
};

static int callback(void *t_o_p, int col_count, char **col_data, char **col_name) {

	table_output *out = static_cast<table_output*>(t_o_p);
	if( !(*out).column_name.size() ) {
		for(int i = 0; i < col_count; i++) {
			(*out).column_name.push_back(col_name[i]);
			(*out).max_column_width.push_back(strlen(col_name[i]));
		}
	}

	vector<string> r;
	for(int i = 0; i < col_count; i++) {
		r.push_back(col_data[i]);
		if(strlen(col_data[i]) > (*out).max_column_width[i]) {
			(*out).max_column_width[i] = strlen(col_data[i]);
		}
	}
	(*out).column_data.push_back(r);

	return 0;
}

int main() {

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Meteoroid");
	window.setVerticalSyncEnabled(true);

	sf::Clock clock;

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("test.db", &db);

    if( rc ) {

    	printf("Can't open database: %s\n", sqlite3_errmsg(db));
    	return 1;

	} else {

	   printf("Opened database successfully\n");

	}


	sf::Font font;

	if(!font.loadFromFile("Assets/crystal.ttf")) {
		cout << "Error loading font" << endl;
	}

	sf::Text text;
	sf::String str = "";
	text.setFont(font);
	text.setString(str);
	text.setCharacterSize(16);
	text.setFillColor(sf::Color::White);

	sf::Text eot_text;
	eot_text.setFont(font);
	eot_text.setCharacterSize(16);
	eot_text.setFillColor(sf::Color::White);
	eot_text.setPosition(0.f, 720.f - 20.f);

	sf::Text time_text;
	time_text.setFont(font);
	time_text.setCharacterSize(16);
	time_text.setFillColor(sf::Color::White);

	sf::RectangleShape carrot(sf::Vector2f(2,20));

	sf::Time time;

	float cb = 0;
	uint64_t cp = str.getSize();
	table_output x;

	bool show_table = false;

	while (window.isOpen()) {

		time = clock.restart();
		cb += time.asSeconds() * 2;
		time_text.setString(to_string(cb).substr(0, int( log(cb)/log(10) ) + 5));
		time_text.setPosition(1280.f - ((int(log(cb)/log(10)) + 5) * 8), 0);

		if(int(cb) % 2) {
			carrot.setFillColor(sf::Color::White);
		} else {
			carrot.setFillColor(sf::Color::Transparent);
			if(cb >= 2) {
				cb = 0;
			}
		}

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {

				window.close();
				sqlite3_close(db);

			}

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && cp) {
				cp--;
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && cp < str.getSize()) {
				cp++;
			}

			if (event.type == sf::Event::TextEntered) {
				uint8_t te = event.text.unicode;
			    if (31 < te && te < 128) {

					str.insert(cp, static_cast<char>(event.text.unicode));
					cp++;
					cb = 1;

				} else if(te == 8 && str.getSize()) {

					str.erase(cp-1, 1);
					cp--;
					cb = 1;

				} else if(te == 13) {

					str.insert(cp, "\n");
					cp++;
					cb = 1;
					if(str.substring(0,5) == "SQL: ") {

						rc = sqlite3_exec(db, str.substring(5).toAnsiString().c_str(), callback, &x, &zErrMsg);
					    if( rc!=SQLITE_OK ) {
							printf("SQL error: %s\n", zErrMsg);
					    	sqlite3_free(zErrMsg);
						}

						show_table = true;

					}


				}
				text.setString(str);
			}

		}

		carrot.setPosition(text.findCharacterPos(cp));

		window.clear();
		window.draw(text);
		window.draw(time_text);
		window.draw(eot_text);
		window.draw(carrot);
		if(show_table) {
			size_t total_row_width = 0;
			for(int i = 0; i < x.column_name.size(); i++) {
				sf::RectangleShape rec(sf::Vector2f(x.max_column_width[i]*8 + 8,24));
				rec.setPosition(sf::Vector2f(640 + total_row_width, 10));
				rec.setFillColor(sf::Color::Black);
				rec.setOutlineThickness(1);
				rec.setOutlineColor(sf::Color::White);
				window.draw(rec);
				sf::Text col_text(x.column_name[i], font, 16);
				col_text.setPosition(640 + total_row_width + 4, 12);
				window.draw(col_text);
				total_row_width += x.max_column_width[i]*8 + 8;
			}
			for(int i = 0; i < x.column_data.size(); i++) {
				total_row_width = 0;
				for(int j = 0; j < x.column_name.size(); j++) {
					sf::RectangleShape rec(sf::Vector2f(x.max_column_width[j]*8 + 8,24));
					rec.setPosition(sf::Vector2f(640 + total_row_width, 34 + 24*i));
					rec.setFillColor(sf::Color::Black);
					rec.setOutlineThickness(1);
					rec.setOutlineColor(sf::Color::White);
					window.draw(rec);
					sf::Text col_text(x.column_data[i][j], font, 16);
					col_text.setPosition(640 + total_row_width + 4, 34 + 24*i + 2);
					window.draw(col_text);
					total_row_width += x.max_column_width[j]*8 + 8;
				}
			}
		}
		window.display();

	}

/*

	std::string input;
    for(;;) {
		cout << "Enter Command: ";
	    getline(cin, input);

		if(input == "exit") break;

		int zero = 0;

	    rc = sqlite3_exec(db, input.c_str(), callback, &zero, &zErrMsg);

	    if( rc!=SQLITE_OK ) {
			printf("SQL error: %s\n", zErrMsg);
	    	sqlite3_free(zErrMsg);
		}


    }
	cout << "goodbye";

	cin.get();
*/
    return 0;
}