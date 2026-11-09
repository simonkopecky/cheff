#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#define DYN_ARRAY_CAP 32

// Relative to $HOME
#define RECIPES "/.config/cheff/recipes/"

char* concat_strings(const char *s1, const char *s2) {
	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);

	char *result = (char*)malloc(len1 + len2 + 1);
	
	if (result == NULL) return NULL;

	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1);

	return result;
}

const char* get_home_dir() {
	const char *homedir;

	if ((homedir = getenv("HOME")) == NULL) {
		homedir = getpwuid(getuid())->pw_dir;
	}

	return homedir;
}

bool get_recipe_files(char*** recipe_files, size_t *file_count) {
	if (recipe_files == NULL || file_count == NULL) return false;

	*recipe_files = NULL;
	*file_count = 0;

	const char *home = get_home_dir();
	if (home == NULL) return false;

	char *recipes_path = concat_strings(home, RECIPES);
	if (recipes_path == NULL) return false;

	struct dirent *recipe;
	DIR *dir = opendir(recipes_path);
	free(recipes_path);

	if (dir == NULL) return false;

	char **result = (char**)malloc(sizeof(char**));
	size_t len = 0;
	size_t capacity = DYN_ARRAY_CAP;

	bool success = true;

	while ((recipe = readdir(dir)) != NULL) {
		if (strcmp(recipe->d_name, ".") == 0 || strcmp(recipe->d_name, "..") == 0) continue;

		if (len >= capacity) {
			capacity *= 2;
			char **tmp = (char**)realloc(result, capacity * sizeof(char**));

			if (tmp == NULL) {
				printf("bruh\n");
				success = false;
				break;
			}

			result = tmp;
		}

		char *path = concat_strings(home, RECIPES);
		path = concat_strings(path, recipe->d_name);

		if (path == NULL) {
			success = false;
			break;
		}

		result[len] = path;
		len++;
	}

	closedir(dir);

	if (success) {
		*recipe_files = result;
		*file_count = len;

		return true;
	}
	else {
		for (size_t i = 0; i < len; i++) free(result[i]);
		free(result);

		return false;
	}
}

char** get_item_from_file(char *file_path) {
	if (file_path == NULL) return NULL;

	FILE *file = fopen(file_path, "r");

	if (file == NULL) {
		printf("Couldn't open file %s\n", file_path);
		return NULL;
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

				if (tmp == NULL) return NULL;

				item_name = tmp;
			}

			item_name[item_name_len++] = ch;
		}
	}

	return NULL;
}

int main() {
	char **recipe_files = NULL;
	size_t file_count = 0;

	if (!get_recipe_files(&recipe_files, &file_count)) return 1;

	for (size_t i = 0; i < file_count; i++) {
		printf("%s\n", recipe_files[i]);
	}

	get_item_from_file(recipe_files[0]);

	return 0;
}
