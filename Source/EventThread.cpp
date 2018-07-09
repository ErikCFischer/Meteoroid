//C++ Standard Library
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
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

void eventThread(const thread_info_s& info_arg, const std::mutex& mute_arg, const std::condition_variable& cv_arg) {

	thread_info_s& info = const_cast<thread_info_s&>(info_arg);
	std::mutex& mute = const_cast<std::mutex&>(mute_arg);
	std::condition_variable& cv = const_cast<std::condition_variable&>(cv_arg);

	while (!info.kill) {

		std::unique_lock<std::mutex> lck(mute);
		cv.wait(lck);

		std::cout << "beans" << std::endl;

	}
	
	cv.notify_all();

}

void mdThread(const sf::Texture& texture_md_arg, const std::mutex& mute_arg, const std::condition_variable& cv_arg) {

	sf::Texture& texture_md = const_cast<sf::Texture&>(texture_md_arg);
	std::mutex& mute = const_cast<std::mutex&>(mute_arg);
	std::condition_variable& cv = const_cast<std::condition_variable&>(cv_arg);

	sf::RenderTexture render_md;
	render_md.create(1280, 720);
	
	render_md.clear();
/*
	for (auto path_text : path_md_txt_vctr) {
		render_md.draw(path_text);
	}

	for (auto item_text : item_md_txt_vctr) {
		render_md.draw(item_text);
	}
*/
	render_md.display();

	texture_md = render_md.getTexture();

}