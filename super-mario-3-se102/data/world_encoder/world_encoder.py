import sys
import math
import ntpath
from os import path, walk
from PIL import Image
from abc import ABC, abstractmethod
from enum import Enum

# THIS PROGRAM WILL CONVERT IMAGES OF A WORLD IN SMB3 TO AN ENCODED FORMAT USED IN SMB3 MOCK PROJECT
# INPUT FILES ARE PUT INSIDE THE 'worlds' FOLDER, OUTPUT FILES ARE PUT INSIDE '../worlds' FOLDER
# YOU CAN ALSO MANUALLY CONVERT ONE IMAGE BY GIVING IT AN IMAGE FILE PATH AND (OPTIONAL) AN OUTPUT FILE PATH (.txt FILE)

# World image format:
# image must be a grid of tiles.
# tiles must be taken from 'data/tiles.png'
# in the last row, the first tile will specify the color of the world background,
# this will be use when compare tiles between world image and tiles.png
# if no tile was found to match world image. it's cordinate will be written in unidentifiable_tiles.txt,
# this can be use to spot mistakes in world image or in tile_annotations.png

# world images are taken from https://www.nesmaps.com/maps/SuperMarioBrothers3/SuperMarioBrothers3BG.html#other with modifications
# but you can make your own custom world using tiles in tile_annotations.png


def main():
    # get game dimension from config.txt
    this_dir = path.dirname(path.realpath(__file__))
    config_path = path.join(this_dir, "..", "config.txt")
    game_dim = get_game_dim(config_path)
    if (game_dim == None):
        print("Error: can't find GamePixelDimension value in ../config.txt")
        return

    # automatically encode all images from ./worlds
    if (len(sys.argv) == 1):
        world_path = path.join(this_dir, "worlds")
        _, _, input_img_filenames = next(walk(world_path))

        for filename in input_img_filenames:
            print(f"Working on {filename}...")
            file_path = path.join(world_path, filename)
            world_img = Image.open(file_path)
            output_path = path.join(this_dir, "..", "worlds", path.splitext(filename)[0] + ".txt")
            WorldEncoder(world_img, game_dim, output_path).encode()
            world_img.close()

        print("Done.")

    # get world image path and (optional) output file path from arguments
    else:
        world_img = Image.open(sys.argv[1])

        output_file_path = ""
        if len(sys.argv) > 3:
            output_file_path = sys.argv[3]
        else:
            output_file_path = path.splitext(ntpath.basename(sys.argv[1]))[0] + ".txt"

        print(f"Working on {ntpath.basename(sys.argv[1])}...")
        WorldEncoder(world_img, output_file_path).encode()
        world_img.close()
        print("Done.")


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


class ColorKey:
    TRANSPARENT = (224, 163, 216, 255)  # 'Transparent color' in data/*_annotations.png
    VOID = (69, 42, 0, 255)             # color used to detect next layer
    WALL_TYPE_1 = (204, 185, 215, 255)  # color in data/wall_annotations.png
    WALL_TYPE_2 = (203, 216, 184, 255)  # color in data/wall_annotations.png


class WorldEncoder:
    def __init__(self, world_img, game_dim, output_file_path):
        self.output_file_path = output_file_path
        self.mistake_file = None

        data_dir_path = path.dirname(path.realpath(__file__))
        self.tiles_img = Image.open(path.join(data_dir_path, "data", "tile_annotations.png"))
        self.entities_img = Image.open(path.join(data_dir_path, "data", "entity_annotations.png"))
        self.input_img_pixels = world_img.load()
        self.input_img_tiles_size = (math.floor(world_img.width/16), math.floor(world_img.height/16))
        self.world_tiles_size = (math.floor(world_img.width/16),
                                 math.floor((world_img.height - 4)/(4 * 16)))  # each img contains 4 layers

        self.world_bg_color = self.input_img_pixels[0, 0]
        self.tile_identifier = TileIdentifier(
            self.tiles_img, self.input_img_pixels, ColorKey.TRANSPARENT, self.world_bg_color)
        self.entity_identifier = EntityIdentifier(
            self.entities_img, self.input_img_pixels, ColorKey.TRANSPARENT, self.world_bg_color)

        # spatial partition cell size
        self.sp_cell_dim = (game_dim[0] / 2, game_dim[1] / 2)

    def __del__(self):
        self.tiles_img.close()
        self.entities_img.close()

    def encode(self):
        with open(self.output_file_path, "w+") as encode_file:
            first_layer_line = 1
            second_layer_line = self.world_tiles_size[1] + 2
            third_layer_line = self.world_tiles_size[1]*2 + 3
            fourth_layer_line = self.world_tiles_size[1]*3 + 4
            self._write_world_props_header(encode_file)
            self._encode_world_props(encode_file)

            self._write_encoded_world_header(encode_file)
            self._encode_background(first_layer_line, encode_file)
            self._encode_foreground(second_layer_line, encode_file)

            self._write_grid_props_header(encode_file)
            self._encode_grid_props(encode_file)

            self._write_walls_header(encode_file)
            self._encode_walls(third_layer_line, encode_file)

            self._write_entities_header(encode_file)
            self._encode_entities(fourth_layer_line, encode_file)

        if (self.mistake_file != None):
            self.mistake_file.close()
            self.mistake_file = None

    def _write_world_props_header(self, encode_file):
        encode_file.write("#Dimension (Width, Height)\n")
        encode_file.write("#BackgroundColor (R, G, B)\n")
        encode_file.write("[WORLD PROPERTIES]\n")

    def _encode_world_props(self, encode_file):
        # exclude 2 row of tiles, get height of one of the world in the image.
        encode_file.write(f"{self.world_tiles_size[0]}, {self.world_tiles_size[1]}\n")
        encode_file.write(
            f"{self.world_bg_color[0]}, {self.world_bg_color[1]}, {self.world_bg_color[2]}\n"
        )

    def _write_encoded_world_header(self, encode_file):
        encode_file.write("\n#Background Tile Indices (3 digits Hex)\n")
        encode_file.write("#Foreground Tile Indices (3 digits Hex)\n")
        encode_file.write("[ENCODED WORLD]\n")

    def _encode_background(self, start_line, encode_file):
        for y in range(start_line, start_line + self.world_tiles_size[1]):
            for x in range(self.input_img_tiles_size[0]):
                code = self.tile_identifier.get_tile_code((x, y))
                if code == IdentifierCode.CODE_NOTFOUND:
                    self._write_to_mistake_file((x, y), "tile")
                    encode_file.write("FFF")

                elif code == IdentifierCode.CODE_EMPTY or code == IdentifierCode.CODE_VOID:
                    encode_file.write("FFF")

                else:
                    encode_file.write(code)
        encode_file.write("\n")

    def _encode_foreground(self, start_line, encode_file):
        # same process as background
        return self._encode_background(start_line, encode_file)

    def _write_grid_props_header(self, encode_file):
        encode_file.write("\n#CellDimension (Width, Height)\n")
        encode_file.write("#GridSize (Row, Column)\n")
        encode_file.write("[SPATIAL PARTITION GRID]\n")

    def _encode_grid_props(self, encode_file):
        cell_w = self.sp_cell_dim[0]
        cell_h = self.sp_cell_dim[1]
        grid_col = round(self.world_tiles_size[0] * 16 / cell_w)
        grid_row = round(self.world_tiles_size[1] * 16 / cell_h)
        encode_file.write(f"{cell_w}, {cell_h}\n")
        encode_file.write(f"{grid_col}, {grid_row}\n")

    def _write_walls_header(self, encode_file):
        encode_file.write("\n#WallType, Position (X, Y), Dimension (Width, Height), GridPosition (X, Y)\n")
        encode_file.write("#SpatialGridCellIndex (X, Y), ColumnSpan, RowSpan\n")
        encode_file.write("[WALL ENTITIES]\n")

    def _encode_walls(self, start_line, encode_file):
        end_position = (self.world_tiles_size[0], start_line + self.world_tiles_size[1])
        visited = {}
        for y in range(start_line, start_line + self.world_tiles_size[1]):
            for x in range(self.input_img_tiles_size[0]):
                not_empty_tile = self.tile_identifier.is_empty_tile((x, y)) == False
                not_visited = visited.get((x, y), False) == False
                if (not_empty_tile and not_visited):
                    walL_type, width, height = self._find_max_wall((x, y), end_position, visited)
                    encode_file.write(f"{walL_type.value}, {x*16}, {(y - start_line)*16}, {width}, {height}\n")

                    grid_x, grid_y, c_span, r_span = self._find_cell_positions(
                        start_line*16, (x*16, y*16), (width, height)
                    )
                    encode_file.write(f"{grid_x}, {grid_y}, {c_span}, {r_span}\n")

    def _find_max_wall(self, position, end_position, visited):
        left, top = position
        end_right, end_bottom = end_position
        right = left
        bottom = top
        wall_color = ColorKey.WALL_TYPE_1 if (
            self.input_img_pixels[left*16, top*16] == ColorKey.WALL_TYPE_1) else ColorKey.WALL_TYPE_2

        def is_end(x, y):
            color = self.input_img_pixels[x*16, y*16]
            return visited.get((x, y), False) == True or color != wall_color or color == self.world_bg_color

        found_right = False
        found_bottom = False
        while (not found_right or not found_bottom):
            if bottom == end_bottom:
                bottom -= 1
                found_bottom = True

            if right == end_right:
                right -= 1
                found_right = True

            if not found_bottom:
                for i in range(left, right + 1):
                    if (is_end(i, bottom)):
                        bottom -= 1
                        found_bottom = True
                        break

            if not found_right:
                for i in range(top, bottom + 1):
                    if (is_end(right, i)):
                        right -= 1
                        found_right = True
                        break

            if not found_bottom:
                for i in range(left, right + 1):
                    visited[(i, bottom)] = True

            if not found_right:
                for i in range(top, bottom + 1):
                    visited[(right, i)] = True

            if not found_bottom:
                bottom += 1

            if not found_right:
                right += 1

        wall_type = EntityCode.WALL_TYPE_1 if (wall_color == ColorKey.WALL_TYPE_1) else EntityCode.WALL_TYPE_2
        width = (right - left + 1)*16
        height = (bottom - top + 1)*16
        return (wall_type, width, height)

    def _find_cell_positions(self, grid_start_y, position, dim=(0, 0)):
        px, py = position
        w, h = dim
        cell_w, cell_h = self.sp_cell_dim
        cell_x = math.floor(px / cell_w)
        cell_x_end = math.floor((px + w - 1) / cell_w)
        cell_y = math.floor((py - grid_start_y) / cell_h)
        cell_y_end = math.floor((py + h - 1 - grid_start_y) / cell_h)
        c_span = cell_x_end - cell_x + 1
        r_span = cell_y_end - cell_y_end + 1
        return (cell_x, cell_y, c_span, r_span)

    def _write_entities_header(self, encode_file):
        encode_file.write("\n#EntityName, [Entity specific properties...], Position (X, Y)\n")
        encode_file.write("#SpatialGridCellIndex (X, Y)\n")
        encode_file.write("[WORLD ENTITIES]\n")

    def _encode_entities(self, start_line, encode_file):
        for y in range(start_line, start_line + self.world_tiles_size[1]):
            for x in range(self.input_img_tiles_size[0]):
                # skip void tiles
                code = self.entity_identifier.get_tile_code((x, y))

                if code == IdentifierCode.CODE_EMPTY or code == IdentifierCode.CODE_VOID:
                    continue

                elif code == IdentifierCode.NOT_IMPLEMENTED:
                    self._write_msg_to_mistake_file(f"entity not implemented at position {x, y} ({x*16, y*16} px)")

                elif code == IdentifierCode.CODE_NOTFOUND:
                    self._write_to_mistake_file((x, y), "entity")

                else:
                    if (code == EntityCode.PIRANHA_GREEN or code == EntityCode.PIRANHA_RED):
                        encode_file.write(f"{code.value}, {x*16 + 8}, {(y - start_line)*16}\n")

                    elif (code == EntityCode.VENUS_GREEN or code == EntityCode.VENUS_RED):
                        encode_file.write(f"{code.value}, {x*16 + 8}, {(y - start_line)*16 + 8}\n")

                    else:
                        encode_file.write(f"{code.value}, {x*16}, {(y - start_line)*16}\n")

                    grid_x, grid_y, _, _ = self._find_cell_positions(start_line*16, (x*16, y*16))
                    encode_file.write(f"{grid_x}, {grid_y}\n")

    def _write_to_mistake_file(self, position, type):
        if (self.mistake_file == None):
            self._create_mistake_file()

        x, y = position
        self.mistake_file.write(f"unidentifiable {type} found at position ({x},{y}) ({x*16}, {y*16} px)\n")
        print(f"unidentifiable {type} found at position ({x},{y}) ({x*16}, {y*16} px)")

    def _write_msg_to_mistake_file(self, msg):
        if (self.mistake_file == None):
            self._create_mistake_file()

        self.mistake_file.write(msg + "\n")
        print(msg + "\n")

    def _create_mistake_file(self):
        file_name = path.splitext(ntpath.basename(self.output_file_path))[0]
        self.mistake_file = open(f"{file_name}_mistakes.txt", "w")
        self.mistake_file.write("Created by world_encoder.py, this file contains all tiles that are unidentifiable or not implemented yet.\n" +
                                "Use this to find mistakes in your image or in world_encoder/data/*.png\n")


class IdentifierCode(Enum):
    NOT_IMPLEMENTED = 1
    CODE_NOTFOUND = 2
    CODE_EMPTY = 3
    CODE_VOID = 4


class Identifier(ABC):
    def __init__(self, tiles_img, world_pixels, tile_transparent_color, world_bg_color):
        self.world_pixels = world_pixels
        self.world_bg_color = world_bg_color
        self.tile_transparent_color = tile_transparent_color

        self.tiles_pixels = tiles_img.load()
        self.tiles_size = (math.floor((tiles_img.width - 1) / 17), math.floor((tiles_img.height - 1) / 17))

    def is_void_tile(self, world_tile_position) -> bool:
        x, y = world_tile_position
        return (self.world_pixels[x*16, y*16] == ColorKey.VOID)

    def get_tile_code(self, world_tile_position) -> str:
        if (self.is_empty_tile(world_tile_position)):
            return IdentifierCode.CODE_EMPTY

        if (self.is_void_tile(world_tile_position)):
            return IdentifierCode.CODE_VOID

        for y in range(self.tiles_size[1]):
            for x in range(self.tiles_size[0]):
                if (self._compare_tile((x, y), world_tile_position)):
                    return self._convertToCode((x, y))

        return IdentifierCode.CODE_NOTFOUND

    @ abstractmethod
    def _convertToCode(self, position) -> str:
        pass

    def is_empty_tile(self, world_tile_position):
        tx, ty = world_tile_position
        for y in range(16):
            for x in range(16):
                world_pixel = self.world_pixels[tx * 16 + x, ty * 16 + y]
                if (world_pixel != self.world_bg_color):
                    return False

        return True

    def _compare_tile(self, tile_position, world_tile_position) -> bool:
        for i in range(16):
            for j in range(16):
                tx = 1 + tile_position[0] * (16 + 1)  # +1 for space between tiles in #_annotations.png
                ty = 1 + tile_position[1] * (16 + 1)
                sx = world_tile_position[0] * 16
                sy = world_tile_position[1] * 16
                tile_pixel = self.tiles_pixels[tx + j,  ty + i]
                world_pixel = self.world_pixels[sx + j, sy + i]

                if (tile_pixel == world_pixel or
                   (tile_pixel == self.tile_transparent_color and world_pixel == self.world_bg_color)):
                    continue

                return False

        return True


class TileIdentifier(Identifier):
    def __init__(self, tiles_img, world_pixels, tile_transparent_color, world_bg_color):
        super().__init__(tiles_img, world_pixels, tile_transparent_color, world_bg_color)

    def _convertToCode(self, position) -> str:
        x, y = position
        return format(x + y * self.tiles_size[0], 'x').upper().zfill(3)


class EntityCode(Enum):
    WALL_TYPE_1 = "WallType1"
    WALL_TYPE_2 = "WallType2"
    GOOMBA = "Goomba, Brown"
    GOOMBA_RED = "Goomba, Red"
    PARA_GOOMBA = "ParaGoomba, Brown"
    PARA_GOOMBA_RED = "ParaGoomba, Red"
    KOOPA = "Koopa, Green"
    KOOPA_RED = "Koopa, Red"
    PARA_KOOPA = "ParaKoopa, Green"
    PARA_KOOPA_RED = "ParaKoopa, Red"
    VENUS_RED = "Venus, Red"
    PIRANHA_GREEN = "Piranha, Green"
    PIRANHA_RED = "Piranha, Red"
    VENUS_GREEN = "Venus, Green"
    GOAL = "Goal"
    COIN = "Coin"
    PORTAL_1 = "Portal, 1"
    PORTAL_2 = "Portal, 2"
    QUESTION_BLOCK_COIN = "QuestionBlock, Coin"
    QUESTION_BLOCK_SUPER_LEAF = "QuestionBlock, SuperLeaf"
    BRICK = "Brick, None"
    BRICK_1UP = "Brick, 1Up"
    BRICK_P_SWITCH = "Brick, PSwitch"
    MARIO = "Mario"


class EntityIdentifier(Identifier):
    _codeByPosition = {
        (0, 0): EntityCode.QUESTION_BLOCK_COIN,
        (1, 0): EntityCode.QUESTION_BLOCK_SUPER_LEAF,
        (0, 1): EntityCode.BRICK,
        (1, 1): EntityCode.BRICK_P_SWITCH,
        (2, 1): EntityCode.BRICK_1UP,
        (0, 2): EntityCode.COIN,
        (1, 2): EntityCode.GOAL,
        (2, 2): EntityCode.PORTAL_1,
        (3, 2): EntityCode.PORTAL_2,
        (0, 3): EntityCode.GOOMBA,
        (1, 3): EntityCode.GOOMBA_RED,
        (2, 3): EntityCode.PARA_GOOMBA,
        (3, 3): EntityCode.PARA_GOOMBA_RED,
        (0, 4): EntityCode.KOOPA,
        (1, 4): EntityCode.PARA_KOOPA,
        (2, 4): EntityCode.KOOPA_RED,
        (0, 5): EntityCode.PIRANHA_RED,
        (1, 5): EntityCode.PIRANHA_GREEN,
        (2, 5): EntityCode.VENUS_GREEN,
        (0, 6): EntityCode.MARIO,
    }

    def __init__(self, tiles_img, world_pixels, tile_transparent_color, world_bg_color):
        super().__init__(tiles_img, world_pixels, tile_transparent_color, world_bg_color)

    def _convertToCode(self, position) -> str:
        return self._codeByPosition.get(position, IdentifierCode.NOT_IMPLEMENTED)


if __name__ == "__main__":
    main()
