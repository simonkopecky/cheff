import os
from pathlib import Path

RECIPES_CONFIG_PATH = str(Path.home()) + "/.config/cheff/recipes/"

def get_files_in_dir(directory: str) -> list:

    result = []
    for (root, dirs, files) in os.walk(directory):
        for f in files:
            result.append(directory + f)

    return result

def get_items_from_file(file_path: str) -> dict:

    result = {}

    key = ""
    val = ""

    in_item = False
    comment = False

    with open(file_path, "r") as f:
        for char in f.read():
            if (char == '#'):
                comment = True

            if (comment):
                if (char == '\n'):
                    comment = False

                continue

            if in_item:
                if char == '}':
                    in_item = False

                    result[key] = val

                    key = ""
                    val = ""
                    continue

                val += char
            else:
                if char in (' ', '\n', '\r'): continue

                if char == '{':
                    in_item = True
                    continue

                key += char

    return result

def main():
    recipe_paths = get_files_in_dir(RECIPES_CONFIG_PATH)

    items = get_items_from_file(recipe_paths[0])

if __name__ == "__main__":
    main()
