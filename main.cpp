#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <iostream>
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

char** get_items_from_file(std::string file_path) {
	if (file_path == nullptr) return nullptr;

	FILE *file = fopen(file_path, "r");

	if (file == nullptr) {
		printf("Couldn't open file %s\n", file_path);
		return nullptr;
	}

	bool in_item = false;
	size_t item_name_len = 0;
	size_t item_name_capacity = DYN_ARRAY_CAP;
	char *item_name = (char*)malloc(item_name_capacity);

	while (!feof(file)) {
		char ch = fgetc(file);

		if (in_item) {
			
		}
		else {
			if (ch == ' ' || ch == '\n') {
				continue;
			}
			else if (ch == '{') {
				in_item = true;
				ch = '\0';
			}

			if (item_name_len >= item_name_capacity) {
				item_name_capacity *= 2;

				char *tmp = (char*)realloc(item_name, item_name_capacity);

				if (tmp == nullptr) return nullptr;

				item_name = tmp;
			}

			item_name[item_name_len++] = ch;
		}
	}

	return nullptr;
}

int main() {
	std::vector<std::string> recipe_files = get_recipe_files();

	if (recipe_files.empty()) return 1;

	for (size_t i = 0; i < recipe_files.size(); i++) {
		std::cout << recipe_files[i];
	}

	return 0;
}
