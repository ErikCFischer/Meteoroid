#include <Screens/VirtualSystemScreen.hpp>
#include <Screens/FileSystemScreen.hpp>

#ifdef DEBUG
	#define dbp(x) std::cout << x << std::endl
#else
	#define dbp(x) //x
#endif


//File system library
namespace fs = std::experimental::filesystem;

bool hovered(sf::Text&, sf::RenderWindow&);
bool hovered(sf::Shape&, sf::RenderWindow&);
bool hovered(sf::Text&, sf::RenderWindow&, sf::View&);
bool hovered(sf::Shape&, sf::RenderWindow&, sf::View&);

bool hovered(sf::View&, sf::RenderWindow&);

int main() {

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Meteoroid");
	sf::View total_window_view(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));

	VirtualSystemScreen vss(0, 0, window.getSize().x/2, window.getSize().y);

	FileSystemScreen fss(window.getSize().x/2, 0, window.getSize().x/2, window.getSize().y);
/*
	sf::RenderTexture render_fs;
	render_fs.create(window.getSize().x, window.getSize().y);
	sf::View view_fs(sf::FloatRect(0, 0, window.getSize().x/2, window.getSize().y));
	view_fs.setViewport(sf::FloatRect(0.5, 0, 0.5, 1));
*/

	sf::Clock clock;

	sf::Font font;

	if(!font.loadFromFile("Assets/crystal.ttf")) {
		dbp("Error loading font");
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

	uint8_t output_format = 4;
	uint8_t context_menu = 0;
	sf::Vector2f context_menu_pos;

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

			if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {

				window.close();

			}

			if (event.type == sf::Event::Resized) {
				
				total_window_view = sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height));

			}

			if(event.type == sf::Event::MouseMoved) {

				vss.updateCursorPos(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
				fss.updateCursorPos(window.mapPixelToCoords(sf::Mouse::getPosition(window)));

			}

			if(event.type == sf::Event::MouseButtonPressed) {
				if(event.mouseButton.button == sf::Mouse::Left) {

					context_menu = 0;

					vss.updatePath();
					fss.updatePath();

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

					output_format = 0;

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
				/*
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
				*/
			}

		}

		carrot.setPosition(user_text.findCharacterPos(carrot_position));

		window.clear();
		//render_fs.clear();

		if(output_format == 0) {

			for(int i = 0; i < text_archive.size(); i++) {
				window.draw(text_archive[i]);
			}

			window.draw(user_text);
			window.draw(carrot);

		} 
		if(output_format == 1) {
/*
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
*/
		}
		if(output_format == 2 || output_format == 4) {
			window.setView(total_window_view);
			window.draw(fss);
		}
		if(output_format == 3 || output_format == 4) {
			window.setView(total_window_view);
			window.draw(vss);
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

	return 0;

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