
import sys
from os import path, walk
from PIL import Image
from src.world_encoder import WorldEncoder
from src.world_map_encoder import WorldMapEncoder
from src.entity_anno_map import world_entity_anno_map, world_map_entity_anno_map

# THIS PROGRAM WILL CONVERT IMAGES OF A WORLD/WORLD MAP IN SMB3 TO AN ENCODED FORMAT USED IN SMB3 MOCK PROJECT
# BY DEFAULT, INPUT WORLD IMAGES ARE PUT INSIDE THE 'worlds' FOLDER, OUTPUT FILES ARE PUT INSIDE '../worlds' FOLDER
# BY DEFAULT, INPUT MAP IMAGES ARE PUT INSIDE THE 'maps' FOLDER, OUTPUT FILES ARE PUT INSIDE '../maps' FOLDER

# INPUT FILE MUST FOLLOW THIS NAMING CONVENTION:
# World map image name: wm_{number}.png
# world image name: w_{world map number}_{world number}_{scene number}.png
# image file can only use tiles taken from "data/*.png"


def main():
    # get game dimension from config.txt
    this_dir = path.dirname(sys.argv[0])
    config_path = path.join(this_dir, "..", "config.txt")
    game_dim = get_game_dim(config_path)
    if (game_dim == None):
        print("Error: can't find GamePixelDimension value in ../config.txt")
        return

    default_output_world_dir = path.join(this_dir, "..", "worlds")
    default_output_world_map_dir = path.join(this_dir, "..", "world_maps")

    default_world_tile_anno_path = path.join(this_dir, "data", "world_tile_annotations.png")
    default_world_entity_anno_path = path.join(this_dir, "data", "world_entity_annotations.png")

    default_world_map_tile_anno_path = path.join(this_dir, "data", "world_map_tile_annotations.png")
    default_world_map_entity_anno_path = path.join(this_dir, "data", "world_map_entity_annotations.png")
    default_world_map_node_anno_path = path.join(this_dir, "data", "world_map_node_annotations.png")

    # automatically encode all images from ./worlds
    if len(sys.argv) == 1:
        default_input_worlds_dir = path.join(this_dir, "worlds")
        encoding_world_files_form_folder(default_input_worlds_dir,
                                         default_output_world_dir,
                                         game_dim,
                                         default_world_tile_anno_path,
                                         default_world_entity_anno_path,
                                         world_entity_anno_map)

        default_input_world_maps_dir = path.join(this_dir, "world_maps")
        encoding_world_map_files_form_folder(default_input_world_maps_dir,
                                             default_output_world_map_dir,
                                             game_dim,
                                             default_world_map_tile_anno_path,
                                             default_world_map_entity_anno_path,
                                             world_map_entity_anno_map,
                                             default_world_map_node_anno_path)
        print("Done.")

    # manually provide image_type and target folder
    elif len(sys.argv) == 3:
        input_folder_dir = sys.argv[1]
        image_type = sys.argv[2]
        if (image_type == "-world"):
            encoding_world_files_form_folder(input_folder_dir,
                                             default_output_world_dir,
                                             game_dim,
                                             default_world_tile_anno_path,
                                             default_world_entity_anno_path,
                                             world_entity_anno_map)
            print("Done.")

        elif (image_type == "-worldmap"):
            encoding_world_map_files_form_folder(input_folder_dir,
                                                 default_output_world_map_dir,
                                                 game_dim,
                                                 default_world_map_tile_anno_path,
                                                 default_world_map_entity_anno_path,
                                                 world_map_entity_anno_map,
                                                 default_world_map_node_anno_path)
            print("Done.")

        else:
            print("Error: Invalid ImageType")

    else:
        print("Error: invalid amount of input arguments")


def encoding_world_files_form_folder(input_folder_dir, output_folder_dir, game_dim, tile_anno_path, entity_anno_path, entity_anno_map):
    _, _, input_img_filenames = next(walk(input_folder_dir))

    for filename in input_img_filenames:
        print(f"Working on {filename}...")
        file_path = path.join(input_folder_dir, filename)
        world_img = Image.open(file_path)
        output_path = path.join(output_folder_dir, path.splitext(filename)[0] + ".txt")

        WorldEncoder(world_img, game_dim, output_path, tile_anno_path, entity_anno_path, entity_anno_map).encode()

        world_img.close()


def encoding_world_map_files_form_folder(input_folder_dir, output_folder_dir, game_dim, tile_anno_path,
                                         entity_anno_path, entity_anno_map, node_anno_path):
    _, _, input_img_filenames = next(walk(input_folder_dir))

    for filename in input_img_filenames:
        print(f"Working on {filename}...")
        file_path = path.join(input_folder_dir, filename)
        world_img = Image.open(file_path)
        output_path = path.join(output_folder_dir, path.splitext(filename)[0] + ".txt")

        WorldMapEncoder(world_img, game_dim, output_path, tile_anno_path,
                        entity_anno_path, entity_anno_map, node_anno_path).encode()

        world_img.close()


def get_game_dim(path):
    with open(path, "r") as config_file:
        while (True):
            line = get_next_non_comment_line(config_file)
            if line == "EOF":
                break

            if line != '[GAME SETTINGS]\n':
                continue

            tokens = get_next_non_comment_line(config_file).split('#')[0].split(',')
            if (len(tokens) != 2):
                print("Error: expected 2 tokens from GamePixelDimension in ../config.txt")
                return None

            return (int(tokens[0]), int(tokens[1]))

    return None


def get_next_non_comment_line(file) -> str:
    while (True):
        line = file.readline()
        if not line:
            return "EOF"

        if line[0] != '#' and line != "":
            return line


if __name__ == "__main__":
    main()
