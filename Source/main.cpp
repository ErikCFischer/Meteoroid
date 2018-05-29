#include <iostream>
#include <string>
#include <stdio.h>
#include <sqlite3.h>

#include <SFML/Graphics.hpp>

using namespace std;

static int callback(void *row, int col_count, char **col_data, char **col_name) {

	int *r = (int*)row;

	if(*r == 0) {
		printf("-");
		for(int i = 0; i < col_count; i++) {
			printf("-------------");
		}
		printf("\n");

		printf("|");
		for(int i = 0; i < col_count; i++) {
			printf(" %10s |", col_name[i]);
		}
		printf("\n");

		printf("-");
		for(int i = 0; i < col_count; i++) {
			printf("-------------");
		}
		printf("\n");
	}

	printf("|");
	for(int i = 0; i < col_count; i++) {
		printf(" %10s |", col_data[i]);
	}
	printf("\n");

	cout << endl;
	(*r)++;
	return 0;
}

int main() {

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

    sqlite3_close(db);

	cout << "goodbye";

	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
