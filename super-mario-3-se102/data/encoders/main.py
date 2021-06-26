
import sys
from os import path, walk
from PIL import Image

from src.world_encoder import WorldEncoder
from src.map_encoder import MapEncoder
from src.entity_anno_map import world_entity_anno_map, world_map_entity_anno_map


def main():
    # get game dimension from config.txt
    this_dir = path.dirname(sys.argv[0])
    config_path = path.join(this_dir, "..", "config.txt")
    game_dim = get_game_dim(config_path)
    if (game_dim == None):
        print("Error: can't find GamePixelDimension value in ../config.txt")
        return

    default_output_world_dir = path.join(this_dir, "..", "worlds")
    default_output_world_map_dir = path.join(this_dir, "..", "world maps")

    default_world_tile_anno_path = path.join(this_dir, "data", "world_tile_annotations.png")
    default_world_entity_anno_path = path.join(this_dir, "data", "world_entity_annotations.png")

    default_world_map_tile_anno_path = path.join(this_dir, "data", "world_map_tile_annotations.png")
    default_world_map_entity_anno_path = path.join(this_dir, "data", "world_map_entity_annotations.png")

    # automatically encode all images from ./worlds
    if len(sys.argv) == 1:
        default_input_worlds_dir = path.join(this_dir, "worlds")
        encoding_files_form_folder(
            ImageType.WORLD,
            default_input_worlds_dir,
            default_output_world_dir,
            game_dim,
            default_world_tile_anno_path,
            default_world_entity_anno_path,
            world_entity_anno_map,
        )

        default_input_world_maps_dir = path.join(this_dir, "world maps")
        encoding_files_form_folder(
            ImageType.WORLD_MAP,
            default_input_world_maps_dir,
            default_output_world_map_dir,
            game_dim,
            default_world_map_tile_anno_path,
            default_world_map_entity_anno_path,
            world_map_entity_anno_map,
        )

    # manually provide image_type and target folder
    elif len(sys.argv) == 3:
        image_type = sys.argv[1]
        input_folder_dir = sys.argv[2]
        if (image_type == ImageType.WORLD):
            encoding_files_form_folder(image_type, input_folder_dir, default_output_world_dir,
                                       game_dim, default_world_tile_anno_path, default_world_entity_anno_path,
                                       world_entity_anno_map)
        elif (image_type == ImageType.WORLD_MAP):
            encoding_files_form_folder(image_type, input_folder_dir, default_output_world_dir,
                                       game_dim, default_world_map_tile_anno_path, default_world_map_entity_anno_path,
                                       world_map_entity_anno_map)
        else:
            print("Error: Invalid ImageType")

    else:
        print("Error: invalid amount of input arguments")


def encoding_files_form_folder(img_type, input_folder_dir, output_folder_dir, game_dim, tile_anno_path, entity_anno_path, entity_anno_map):
    _, _, input_img_filenames = next(walk(input_folder_dir))

    for filename in input_img_filenames:
        print(f"Working on {filename}...")
        file_path = path.join(input_folder_dir, filename)
        world_img = Image.open(file_path)
        output_path = path.join(output_folder_dir, path.splitext(filename)[0] + ".txt")

        get_encoder(img_type, world_img, game_dim, output_path,
                    tile_anno_path, entity_anno_path, entity_anno_map).encode()
        world_img.close()

    print("Done.")


class ImageType():
    WORLD = "-world"
    WORLD_MAP = "-map"


def get_encoder(img_type, world_img, game_dim, output_path, tile_anno_path, entity_anno_path, entity_anno_map):
    return {
        ImageType.WORLD: WorldEncoder,
        ImageType.WORLD_MAP: MapEncoder
    }[img_type](world_img, game_dim, output_path, tile_anno_path, entity_anno_path, entity_anno_map)


def get_game_dim(path):
    with open(path, "r") as config_file:
        while (True):
            line = get_next_non_comment_line(config_file)
            if line == "EOF":
                break

            if line != '[GAME SETTINGS]\n':
                continue

            tokens = get_next_non_comment_line(config_file)[:-1].split(',')
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
