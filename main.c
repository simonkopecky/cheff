#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

// Relative to $HOME
#define RECIPES "/.config/cheff/recipes/"

char* concat_strings(const char *s1, const char *s2) {
	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);

	char *result = malloc(len1 + len2 + 1);
	
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

	size_t index = 0;

	char **result = malloc(sizeof(char**));
	bool success = true;

	while ((recipe = readdir(dir)) != NULL) {
		if (strcmp(recipe->d_name, ".") == 0 || strcmp(recipe->d_name, "..") == 0) continue;

		if (index != 0) {
			char **tmp = realloc(result, ++index * sizeof(char**));

			if (tmp == NULL) {
				success = false;
				break;
			}

			result = tmp;
		}

		char *name = strdup(recipe->d_name);
		if (name == NULL) {
			success = false;
			break;
		}

		result[index] = name;
		index++;
	}

	closedir(dir);

	if (success) {
		*recipe_files = result;
		*file_count = index;

		return true;
	}
	else {
		for (size_t i = 0; i < index; i++) free(result[i]);
		free(result);

		return false;
	}
}

int main() {
	char **recipe_files = NULL;
	size_t file_count = 0;

	if (!get_recipe_files(&recipe_files, &file_count)) return 1;

	for (size_t i = 0; i < file_count; i++) {
		printf("%s\n", recipe_files[i]);
	}

	return 0;
}
