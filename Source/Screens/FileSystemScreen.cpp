#include <Screens/FileSystemScreen.hpp>

#ifdef DEBUG
	#define dbp(x) std::cout << x << std::endl
#else
	#define dbp(x) //x
#endif

FileSystemScreen::FileSystemScreen(float set_left, float set_top, float set_width, float set_height) {

	left = set_left;
	top = set_top;
	width = set_width;
	height = set_height;
	
	current_path = fs::current_path();

	file_path_text.clear();
	int left_marg = left;
	for (auto & path_segment : current_path) {

		if(path_segment == "\\") continue;

		if(file_path_text.empty()) {
			file_path_text.emplace_back(path_segment.string(), font, 16);
			file_path_text.back().setPosition(sf::Vector2f(left_marg, top));
		} else {
			file_path_text.emplace_back("/" + path_segment.string(), font, 16);
			file_path_text.back().setPosition(left_marg, top);
		}

		left_marg += file_path_text.back().getString().getSize() * 8;
		file_path_text.back().setOutlineColor(sf::Color::Blue);

	}

	file_item_text.clear();
	int dir_pos = 0;
	for (auto & current_item : fs::directory_iterator(current_path)) {
		switch(current_item.status().type()) {
			case fs::file_type::regular:
				file_item_text.emplace_back(current_item.path().filename().string(), font, 16);
				file_item_text.back().setFillColor(sf::Color(0xAA, 0xFF, 0xAA));
				file_item_text.back().setOutlineColor(sf::Color(0x00, 0xAA, 0x00));
				break;
			case fs::file_type::directory:
				file_item_text.emplace(file_item_text.begin()+dir_pos, current_item.path().filename().string(), font, 16);
				file_item_text[dir_pos].setFillColor(sf::Color(0xAA, 0xAA, 0xFF));
				file_item_text[dir_pos].setOutlineColor(sf::Color(0x00, 0x00, 0xAA));
				dir_pos++;
				break;
			default:
				file_item_text.emplace_back(current_item.path().filename().string(), font, 16);
				file_item_text.back().setFillColor(sf::Color(0xFF, 0xAA, 0xAA));
				file_item_text.back().setOutlineColor(sf::Color(0xAA, 0x00, 0x00));
				break;
		}
	}
	for (int i = 0; i < file_item_text.size(); i++) {
		file_item_text[i].setPosition(left, top + 24 + 24*i);
	}

	if(!font.loadFromFile("Assets/crystal.ttf")) {
		dbp("Error loading font");
	}

}

FileSystemScreen::~FileSystemScreen() {

}

void FileSystemScreen::updateCursorPos(sf::Vector2f cursor_pos_update) {

	cursor_pos = cursor_pos_update;

	for(auto & path_text : file_path_text) {
		if(path_text.getGlobalBounds().contains(cursor_pos)) {
			path_text.setOutlineThickness(1);
		} else if(path_text.getOutlineThickness() == 1) {
			path_text.setOutlineThickness(0);
		}
	}

	for(auto & item_text : file_item_text) {
		if(item_text.getGlobalBounds().contains(cursor_pos)) {
			item_text.setOutlineThickness(1);
		} else if(item_text.getOutlineThickness() == 1) {
			item_text.setOutlineThickness(0);
		}
	}

}

void FileSystemScreen::updatePath() {

	for(auto & path_text : file_path_text) {
		if(path_text.getGlobalBounds().contains(cursor_pos)) {

			if(file_path_text.back().getString() == current_path.root_name().string()) {
				file_item_text.clear();
				file_path_text.clear();
				for(int i = 0; i < 26; i++) {
					std::string drive_letter = "";
					drive_letter += char('A' + i);
					drive_letter += ":";
					if( fs::exists(fs::path(drive_letter)) ) {
						file_item_text.emplace_back(drive_letter, font, 16);
						file_item_text.back().setPosition(left, top + 24 * (file_item_text.size() - 1));
						file_item_text.back().setOutlineColor(sf::Color::Blue);
					}
				}
				current_path = fs::path("");
				break;
			}

			while(file_path_text.back().getString() != path_text.getString()) {
				current_path.remove_filename();
				file_path_text.pop_back();
			}

			break;

		}
	}
	for(auto & item_text : file_item_text) {
		if(item_text.getGlobalBounds().contains(cursor_pos)) {
			
			current_path /= fs::path(std::string(item_text.getString()));

			break;
		}
	}

	sf::Color folder_fill_c(0xA0, 0xA0, 0xF0);
	sf::Color folder_outline_c(0x40, 0x40, 0x60);

	sf::Color playlist_fill_c(0xA0, 0xF0, 0xA0);
	sf::Color playlist_outline_c(0x40, 0x60, 0x40);

	sf::Color file_fill_c(0xF0, 0xA0, 0xA0);
	sf::Color file_outline_c(0x60, 0x40, 0x40);
/*
	file_path_text.clear();
	for (auto & path_segment : current_path) {

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
*/
}

void FileSystemScreen::draw(sf::RenderTarget& target, sf::RenderStates states) const {

	for (auto path_text : file_path_text) {
		target.draw(path_text, states);
	}

	for (auto item_text : file_item_text) {
		target.draw(item_text, states);
	}

}