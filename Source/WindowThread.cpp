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

#include <WindowThread.hpp>

void windowThread(const bool& kill, const std::mutex& mute, const std::condition_variable& cv) {
	sf::RenderWindow window(sf::VideoMode(1280, 720), "New Window");

	bool& kill_ref = const_cast<bool&>(kill);
	std::mutex& mute_ref = const_cast<std::mutex&>(mute);
	std::condition_variable& cv_ref = const_cast<std::condition_variable&>(cv);

	while (window.isOpen()) {

		if(kill_ref) {
			window.close();
		}
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		// clear the window with black color
		window.clear();

		// draw everything here...
		window.draw(sf::CircleShape(50));

		// end the current frame
		window.display();
	}

	std::unique_lock<std::mutex> lck(mute_ref);
	kill_ref = false;
	cv_ref.notify_all();

}