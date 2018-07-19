#pragma once

//C++ Standard Library
#include <iostream>
#include <vector>
#include <string>
//C++17 Experimental Libraries
#include <experimental/filesystem>
//SFML
#include <SFML/Graphics.hpp>

namespace fs = std::experimental::filesystem;

class FileSystemScreen : public sf::Drawable {
	private:
		float left, top, width, height;

		sf::Font font;
		std::vector<sf::Text> file_path_text;
		std::vector<sf::Text> file_item_text;

		sf::Vector2f cursor_pos;

		fs::path current_path;

		virtual void draw(sf::RenderTarget&, sf::RenderStates) const;

	public:
		FileSystemScreen(float,float,float,float);
		~FileSystemScreen();
		
		void updateCursorPos(sf::Vector2f);
		void updatePath();


};