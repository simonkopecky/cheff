#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <iostream>
#include <print>
#include <string>
#include <vector>
#include <optional>

#define DYN_ARRAY_CAP 32

// Relative to $HOME
#define RECIPES "/.config/cheff/recipes/"

std::string get_home_dir() {
	const char *homedir = nullptr;

	if ((homedir = getenv("HOME")) == nullptr) {
		homedir = getpwuid(getuid())->pw_dir;
	}

	if (homedir == nullptr) return "";

	std::string result(homedir);

	return result;
}

std::vector<std::string> get_recipe_files() {
	std::string home = get_home_dir();
	if (home.empty()) return {};

	std::string recipes_path = home + RECIPES;

	struct dirent *ent;
	DIR *dir = opendir(recipes_path.c_str());

	if (dir == nullptr) return {};

	std::vector<std::string> result;

	while ((ent = readdir(dir)) != nullptr) {
		std::string name(ent->d_name);

		if (name == "." || name == "..") continue;

		std::string path = home + RECIPES + name;

		result.push_back(path);
	}

	closedir(dir);

	return result;
}

std::string get_items_from_file(std::string file_path) {
	if(file_path.empty()) return {};

	FILE *file = fopen(file_path.c_str(), "r");

	if (file == nullptr) {
		std::cout << "Couldn't open file " << file_path << "\n";
		return nullptr;
	}

	bool in_item = false;
	std::string item_name;

	while (!feof(file)) {
		char ch = fgetc(file);

		if (in_item) {
			std::cout << item_name << "\n";
			break;
		}
		else {
			if (ch == ' ' || ch == '\n') {
				continue;
			}
			else if (ch == '{') {
				in_item = true;
				ch = '\0';
			}

			item_name.push_back(ch);
		}
	}

	return {};
}

int main() {
	std::vector<std::string> recipe_files = get_recipe_files();

	if (recipe_files.empty()) return 1;

	for (size_t i = 0; i < recipe_files.size(); i++) {
		std::cout << recipe_files[i] << "\n";
	}

	get_items_from_file(recipe_files[0]);

	return 0;
}
