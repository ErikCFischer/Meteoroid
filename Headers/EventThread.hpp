#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

#include <SFML/Graphics.hpp>

struct thread_info_s {
	bool kill = false;
	std::string type;
	union {
		sf::Vector2i mouse_pos;
		bool scroll;
	};
};

void eventThread(const thread_info_s&, const std::mutex&, const std::condition_variable&);

void mdThread(const sf::Texture&, const std::mutex&, const std::condition_variable&);