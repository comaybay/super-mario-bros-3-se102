from .shared_logic import ColorKey, Encoder,  WorldEntityIdentifier, EntityCode
import math

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


class WorldEncoder(Encoder):
    def __init__(self, world_img, game_dim, output_file_path):
        super().__init__(world_img, 4, game_dim, output_file_path)

    def encode(self):
        with open(self.output_file_path, "w+") as encode_file:
            first_layer_line = 1
            second_layer_line = self.layer_tiles_size[1] + 2
            third_layer_line = self.layer_tiles_size[1]*2 + 3
            fourth_layer_line = self.layer_tiles_size[1]*3 + 4
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

    def _write_walls_header(self, encode_file):
        encode_file.write("\n#WallType, Position (X, Y), Dimension (Width, Height), GridPosition (X, Y)\n")
        encode_file.write("#SpatialGridCellIndex (X, Y), ColumnSpan, RowSpan\n")
        encode_file.write("[WALL ENTITIES]\n")

    def _encode_walls(self, start_line, encode_file):
        end_position = (self.layer_tiles_size[0], start_line + self.layer_tiles_size[1])
        visited = {}
        for y in range(start_line, start_line + self.layer_tiles_size[1]):
            for x in range(self.input_img_tiles_size[0]):
                not_empty_tile = self.tile_identifier.is_empty_tile((x, y)) == False
                not_visited = visited.get((x, y), False) == False
                if (not_empty_tile and not_visited):
                    walL_type, w_in_tile, h_in_tile = self._find_max_wall((x, y), end_position, visited)
                    encode_file.write(
                        f"{walL_type.value}, {x*16}, {(y - start_line)*16}, {w_in_tile*16}, {h_in_tile*16}\n")

                    grid_x, grid_y, c_span, r_span = self._find_cell_positions(
                        start_line, (x, y), (w_in_tile, h_in_tile)
                    )
                    encode_file.write(f"{grid_x}, {grid_y}, {c_span}, {r_span}\n")

    def _find_max_wall(self, position_in_tile, end_position_in_tile, visited):
        left, top = position_in_tile
        end_right, end_bottom = end_position_in_tile
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
        w_in_tile = right - left + 1
        h_in_tile = bottom - top + 1
        return (wall_type, w_in_tile, h_in_tile)

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
