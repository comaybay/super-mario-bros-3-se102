import sys
import math
import ntpath
from os import path
from PIL import Image
from .shared_logic import ColorKey, Encoder, IdentifierCode, EntityCode


class MapEncoder(Encoder):
    def __init__(self, world_img, game_dim, output_file_path, tile_anno_path, entity_anno_path, entity_anno_map):
        super().__init__(world_img, 4, game_dim, output_file_path, tile_anno_path, entity_anno_path, entity_anno_map)

    def encode(self):
        with open(self.output_file_path, "w+") as encode_file:
            first_layer_line = 1
            second_layer_line = self.layer_tiles_size[1] + 2
            third_layer_line = self.layer_tiles_size[1]*2 + 3
            fourth_layer_line = self.layer_tiles_size[1]*3 + 4
            self._write_world_props_header(encode_file)
            self._encode_world_props(encode_file)

            self._write_encoded_world_header(encode_file)
            self._encode_world_tiles(first_layer_line, encode_file)
            self._encode_world_tiles(second_layer_line, encode_file)

            self._write_grid_props_header(encode_file)
            self._encode_grid_props(encode_file)

            self._write_entities_header(encode_file)
            self._encode_entities(third_layer_line, encode_file)

            self._write_nodes_header(encode_file)
            self._encode_nodes(fourth_layer_line, encode_file)

        if (self.mistake_file != None):
            self.mistake_file.close()
            self.mistake_file = None

    def _write_nodes_header(self, encode_file):
        encode_file.write("\n#NodeId...\n")
        encode_file.write("#NodeId, ContentPath (relative to Root), TopNodeId, LeftNodeId, DownNodeId, RightNodeId\n")
        encode_file.write("[NODES]\n")

    def _encode_nodes(self, second_layer_line, encode_file):
        pass
