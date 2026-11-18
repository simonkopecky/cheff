#include <pwd.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <stdexcept>

// Relative to $HOME
#define RECIPES_CONFIG "/.config/cheff/recipes/"
#define RECIPES_LOCAL "/.local/share/cheff/recipes"

std::filesystem::path get_home_dir() {
	struct passwd *user_entry = getpwuid(getuid());

	if (user_entry == nullptr || user_entry->pw_dir == nullptr) {
		throw std::runtime_error("Couldn't find user's home directory.");
	}

	return user_entry->pw_dir;
}

std::vector<std::string> get_recipe_files() {
	std::string home = get_home_dir();
	if (home.empty()) return {};

	std::string recipes_path = home + RECIPES_CONFIG;

	struct dirent *ent;
	DIR *dir = opendir(recipes_path.c_str());

	if (dir == nullptr) return {};

	std::vector<std::string> result;

	while ((ent = readdir(dir)) != nullptr) {
		std::string name(ent->d_name);

		if (name == "." || name == "..") continue;

		std::string path = home + RECIPES_CONFIG + name;

		result.push_back(path);
	}

	closedir(dir);

	return result;
}

bool skip_comment_line(FILE **file) {
	char ch;
	while ((ch = fgetc(*file)) != '\n') {
		if (feof(*file)) return false;
	}

	return true;
}

std::unordered_map<std::string,std::string> get_items_from_file(std::string file_path) {
	if(file_path.empty()) return {};

	FILE *file = fopen(file_path.c_str(), "r");

	if (file == nullptr) {
		std::cout << "Couldn't open file " << file_path << "\n";
		return {};
	}

	bool in_item = false;

	std::unordered_map<std::string,std::string> items;

	std::string current_data;
	std::string current_key;

	while (!feof(file)) {
		char ch = fgetc(file);

		if (ch == '#') {
			if (!skip_comment_line(&file)) return {};

			continue;
		}

		if (in_item) {
			if (ch == '}') {
				in_item = false;

				items[current_key] = current_data;
				current_key = "";
				current_data = "";

				continue;
			}

			current_data.push_back(ch);
		}
		else {
			if (ch == ' ' || ch == '\n') {
				continue;
			}
			else if (ch == '{') {
				in_item = true;
				continue;
			}

			current_key.push_back(ch);
		}
	}

	return items;
}

int main() {
	std::vector<std::string> recipe_files = get_recipe_files();

	if (recipe_files.empty()) return 1;

	std::unordered_map<std::string,std::string> items = get_items_from_file(recipe_files[0]);

	return 0;
}
