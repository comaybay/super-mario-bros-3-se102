from abc import ABC, abstractmethod
from enum import Enum
import math
import sys
import ntpath
from PIL import Image
import re
import os
from datetime import datetime


class ColorKey:
    TRANSPARENT = (224, 163, 216, 255)  # 'Transparent color' in data/*_annotations.png
    VOID = (69, 42, 0, 255)             # color used to detect next layer
    WALL_TYPE_1 = (204, 185, 215, 255)  # color in data/wall_annotations.png
    WALL_TYPE_2 = (203, 216, 184, 255)  # color in data/wall_annotations.png


class Encoder(ABC):
    def __init__(self, scene_type, input_img, number_of_layers, game_dim, output_file_path, tile_anno_path, entity_anno_path, entity_anno_map):
        self.scene_type = scene_type
        self.output_file_path = output_file_path

        file_name = os.path.splitext(ntpath.basename(self.output_file_path))[0]
        self.mistake_file_path = os.path.join(os.path.dirname(sys.argv[0]), f"{file_name}_mistakes.txt")
        self.mistake_file = self._create_mistake_file(self.mistake_file_path)
        self.has_mistake = False

        input_img_file_name = os.path.split(input_img.filename)[1]
        self.world_map_number = re.search(r"^wm?_(\d+)", input_img_file_name).group(1)
        if self.world_map_number == None:
            raise Exception('Invalid input file name. Expected "wm_{number}.png or w_{number}_{number}_{number}.png"')

        self.tile_anno_img = Image.open(tile_anno_path)
        self.entity_anno_img = Image.open(entity_anno_path)
        self.input_img_pixels = input_img.load()
        self.input_img_tiles_size = (math.floor(input_img.width/16), math.floor(input_img.height/16))
        self.layer_tiles_size = (math.floor(input_img.width/16),
                                 math.floor((input_img.height - number_of_layers)/(number_of_layers * 16)))  # each img contains 4 layers

        self.world_bg_color = self.input_img_pixels[0, 0]
        self.tile_identifier = TileIdentifier(
            self.tile_anno_img, self.input_img_pixels, ColorKey.TRANSPARENT, self.world_bg_color
        )
        self.entity_identifier = EntityIdentifier(
            self.entity_anno_img, entity_anno_map, self.input_img_pixels, ColorKey.TRANSPARENT, self.world_bg_color
        )

        # spatial partition cell size
        self.sp_cell_dim = (game_dim[0] / 2, game_dim[1] / 2)

    def __del__(self):
        self.mistake_file.close()
        if self.has_mistake == False:
            os.remove(self.mistake_file_path)

        self.tile_anno_img.close()
        self.entity_anno_img.close()

    def _create_mistake_file(self, file_path):
        mistake_file = open(file_path, "w")

        date = datetime.now().strftime('%m/%d/%Y %H:%M:%S')
        mistake_file.write(f"{date}\n")
        mistake_file.write("Created by world_encoder.py, this file contains all tiles that are unidentifiable or not implemented yet.\n" +
                           "Use this to find mistakes in your image or in world_encoder/data/*.png\n")
        mistake_file.write("----------------------------------------------------\n")
        return mistake_file

    def _write_world_props_header(self, encode_file):
        encode_file.write("#SceneType (World or WorldMap)\n")
        encode_file.write("#Dimension (Width, Height)\n")
        encode_file.write("#BackgroundColor (R, G, B)\n")
        encode_file.write("#PreviousScenePath (Relative to Root)\n")
        encode_file.write("[WORLD PROPERTIES]\n")

    def _encode_world_props(self, encode_file):
        # exclude 2 row of tiles, get height of one of the world in the image.
        encode_file.write(f"{self.scene_type}\n")
        encode_file.write(f"{self.layer_tiles_size[0]}, {self.layer_tiles_size[1]}\n")
        encode_file.write(
            f"{self.world_bg_color[0]}, {self.world_bg_color[1]}, {self.world_bg_color[2]}\n"
        )
        encode_file.write(f"{self._get_prev_scene_path()}\n")

    @ abstractmethod
    def _get_prev_scene_path(self):
        pass

    def _write_encoded_world_header(self, encode_file):
        encode_file.write("\n#Background Tile Indices (3 digits Hex)\n")
        encode_file.write("#Foreground Tile Indices (3 digits Hex)\n")
        encode_file.write("[ENCODED WORLD]\n")

    def _encode_world_tiles(self, start_line, encode_file):
        for y in range(start_line, start_line + self.layer_tiles_size[1]):
            for x in range(self.input_img_tiles_size[0]):
                code = self.tile_identifier.get_tile_code((x, y))
                if code == IdentifierCode.CODE_NOTFOUND:
                    self._write_to_mistake_file_formated((x, y), "tile")
                    encode_file.write("FFF")

                elif code == IdentifierCode.CODE_EMPTY or code == IdentifierCode.CODE_VOID:
                    encode_file.write("FFF")

                else:
                    encode_file.write(code)
        encode_file.write("\n")

    def _write_grid_props_header(self, encode_file):
        encode_file.write("\n#CellDimension (Width, Height)\n")
        encode_file.write("#GridSize (Column, Row)\n")
        encode_file.write("[SPATIAL PARTITION GRID]\n")

    def _encode_grid_props(self, encode_file):
        cell_w = self.sp_cell_dim[0]
        cell_h = self.sp_cell_dim[1]
        grid_col = round(self.layer_tiles_size[0] * 16 / cell_w)
        grid_row = round(self.layer_tiles_size[1] * 16 / cell_h)
        encode_file.write(f"{cell_w}, {cell_h}\n")
        encode_file.write(f"{grid_col}, {grid_row}\n")

    def _find_cell_positions(self, grid_start_tile_y, position_in_tile, dim_in_tile=(0, 0)):
        grid_start_y = grid_start_tile_y * 16
        px, py = (position_in_tile[0] * 16, position_in_tile[1] * 16)
        w, h = (dim_in_tile[0]*16, dim_in_tile[1]*16)

        cell_w, cell_h = self.sp_cell_dim
        cell_x = math.floor(px / cell_w)
        cell_x_end = math.floor((px + w - 1) / cell_w)
        cell_y = math.floor((py - grid_start_y) / cell_h)
        cell_y_end = math.floor((py + h - 1 - grid_start_y) / cell_h)
        c_span = cell_x_end - cell_x + 1
        r_span = cell_y_end - cell_y_end + 1
        return (cell_x, cell_y, c_span, r_span)

    def _write_entities_header(self, encode_file):
        encode_file.write("\n#EntityName, [Entity specific properties...], Position (X, Y), IsInAnyGrid\n")
        encode_file.write("#[SpatialPartitionGridCellIndex (X, Y) (If IsInAnyGrid = True)]\n")
        encode_file.write("[WORLD ENTITIES]\n")

    def _encode_entities(self, start_line, encode_file):
        for y in range(start_line, start_line + self.layer_tiles_size[1]):
            for x in range(self.input_img_tiles_size[0]):
                # skip void tiles
                code = self.entity_identifier.get_tile_code((x, y))

                if code == IdentifierCode.CODE_EMPTY or code == IdentifierCode.CODE_VOID:
                    continue

                elif code == IdentifierCode.NOT_IMPLEMENTED:
                    self._write_to_mistake_file(f"entity not implemented at position {x, y} ({x*16}, {y*16} px)\n")

                elif code == IdentifierCode.CODE_NOTFOUND:
                    self._write_to_mistake_file_formated((x, y), "entity")

                else:
                    pos_x = x * 16
                    pos_y = (y - start_line) * 16
                    isInAnyGrid = True

                    if (code == EntityCode.PIRANHA_GREEN or code == EntityCode.PIRANHA_RED):
                        pos_x += 8

                    elif (code == EntityCode.VENUS_GREEN or code == EntityCode.VENUS_RED):
                        pos_x += 8
                        pos_y += 8

                    elif (code == EntityCode.MARIO):
                        isInAnyGrid = False

                    elif (code == EntityCode.KOOPA_RED_AND_COIN):
                        encode_file.write(f"{EntityCode.KOOPA_RED.value}, {pos_x}, {pos_y}, {isInAnyGrid}\n")
                        encode_file.write(f"{EntityCode.COIN.value}, {pos_x}, {pos_y}, {isInAnyGrid}\n")
                        continue

                    encode_file.write(f"{code.value}, {pos_x}, {pos_y}, {isInAnyGrid}\n")
                    if isInAnyGrid:
                        grid_x, grid_y, _, _ = self._find_cell_positions(start_line, (x, y))
                        encode_file.write(f"{grid_x}, {grid_y}\n")

    def _write_to_mistake_file_formated(self, position_in_tile, type):
        self.has_mistake = True
        x, y = position_in_tile
        self.mistake_file.write(f"unidentifiable {type} found at position ({x},{y}) ({x*16}, {y*16} px)\n")
        print(f"unidentifiable {type} found at position ({x},{y}) ({x*16}, {y*16} px)")

    def _write_to_mistake_file(self, msg):
        self.has_mistake = True
        self.mistake_file.write(msg)
        print(msg, end='')


class IdentifierCode(Enum):
    NOT_IMPLEMENTED = 1
    CODE_NOTFOUND = 2
    CODE_EMPTY = 3
    CODE_VOID = 4


class Identifier(ABC):
    def __init__(self, tile_anno_img, target_img_pixels, anno_img_transparent_color, target_img_bg_color):
        self.target_img_pixels = target_img_pixels
        self.target_img_bg_color = target_img_bg_color
        self.tile_anno_img_transparent_color = anno_img_transparent_color

        self.ref_tiles_tiles_pixels = tile_anno_img.load()
        self.ref_tiles_img_size = (math.floor((tile_anno_img.width - 1) / 17),
                                   math.floor((tile_anno_img.height - 1) / 17))

    def is_void_tile(self, world_position_in_tile) -> bool:
        x, y = world_position_in_tile
        return (self.target_img_pixels[x*16, y*16] == ColorKey.VOID)

    def get_tile_code(self, world_position_in_tile) -> str:
        if (self.is_empty_tile(world_position_in_tile)):
            return IdentifierCode.CODE_EMPTY

        if (self.is_void_tile(world_position_in_tile)):
            return IdentifierCode.CODE_VOID

        for y in range(self.ref_tiles_img_size[1]):
            for x in range(self.ref_tiles_img_size[0]):
                if (self._compare_tile((x, y), world_position_in_tile)):
                    return self._convertToCode((x, y))

        return IdentifierCode.CODE_NOTFOUND

    @ abstractmethod
    def _convertToCode(self, position_in_tile) -> str:
        pass

    def is_empty_tile(self, world_position_in_tile):
        tx, ty = world_position_in_tile
        for y in range(16):
            for x in range(16):
                world_pixel = self.target_img_pixels[tx * 16 + x, ty * 16 + y]
                if (world_pixel != self.target_img_bg_color):
                    return False

        return True

    def _compare_tile(self, position_in_tile, world_position_in_tile) -> bool:
        for i in range(16):
            for j in range(16):
                tx = 1 + position_in_tile[0] * (16 + 1)  # +1 for space between tiles in #_annotations.png
                ty = 1 + position_in_tile[1] * (16 + 1)
                sx = world_position_in_tile[0] * 16
                sy = world_position_in_tile[1] * 16
                tile_pixel = self.ref_tiles_tiles_pixels[tx + j,  ty + i]
                world_pixel = self.target_img_pixels[sx + j, sy + i]

                if (tile_pixel == world_pixel or
                   (tile_pixel == self.tile_anno_img_transparent_color and world_pixel == self.target_img_bg_color)):
                    continue

                return False

        return True


class TileIdentifier(Identifier):
    def __init__(self, tile_anno_img, target_img_pixels, tile_anno_img_transparent_color, target_img_bg_color):
        super().__init__(tile_anno_img, target_img_pixels, tile_anno_img_transparent_color, target_img_bg_color)

    def _convertToCode(self, position_in_tile) -> str:
        x, y = position_in_tile
        return format(x + y * self.ref_tiles_img_size[0], 'x').upper().zfill(3)


class EntityCode(Enum):
    # WORLD
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
    KOOPA_RED_AND_COIN = "ParaKoopaRedAndCoin"
    VENUS_RED = "Venus, Red"
    PIRANHA_GREEN = "Piranha, Green"
    PIRANHA_RED = "Piranha, Red"
    VENUS_GREEN = "Venus, Green"
    BOOMERANG_BRO = "BoomerangBro"
    GOAL_ROULETTE = "GoalRoulette"
    COIN = "Coin"
    PORTAL_1 = "Portal, 1"
    PORTAL_2 = "Portal, 2"
    QUESTION_BLOCK_COIN = "QuestionBlock, Coin"
    QUESTION_BLOCK_SUPER_LEAF = "QuestionBlock, SuperLeaf"
    BRICK = "Brick, None"
    BRICK_1UP = "Brick, 1Up"
    BRICK_P_SWITCH = "Brick, PSwitch"
    BRICK_SUPER_LEAF = "Brick, SuperLeaf"
    COIN_BLOCK_BRICK = "CoinBlock, Brick"
    MARIO = "Mario"
    NOTE_BLOCK = "NoteBlock, None"
    SUPER_NOTE_BLOCK = "SuperNoteBlock"
    ITEM_WOOD_BLOCK_SUPER_LEAF = "ItemWoodBlock, SuperLeaf"
    # WORLD MAP
    WM_BUSH = "WMBush"
    WM_HELP_BUBBLE = "WMHelpBubble"


class EntityIdentifier(Identifier):

    def __init__(self, entity_anno_img, entity_anno_map, target_img_pixels, entity_anno_img_transparent_color, target_img_bg_color):
        super().__init__(entity_anno_img, target_img_pixels, entity_anno_img_transparent_color, target_img_bg_color)
        self._codeByPosition = entity_anno_map

    def _convertToCode(self, position_in_tile) -> str:
        return self._codeByPosition.get(position_in_tile, IdentifierCode.NOT_IMPLEMENTED)
