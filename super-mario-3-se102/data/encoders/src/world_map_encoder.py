from enum import Enum
import os
from PIL import Image
import re
from .shared_logic import Encoder, Identifier, IdentifierCode, ColorKey


class NodeType(Enum):
    PATH = 0
    STOP = 1
    LEVEL = 2
    LOCK = 3
    START = 4


class WorldMapEncoder(Encoder):
    def __init__(self, input_img, game_dim, output_file_path, tile_anno_path, entity_anno_path, entity_anno_map, node_anno_path):
        super().__init__(input_img, 4, game_dim, output_file_path, tile_anno_path, entity_anno_path, entity_anno_map)
        node_anno_img = Image.open(node_anno_path)
        self.node_identifier = NodeIdentifier(node_anno_img, self.input_img_pixels,
                                              ColorKey.TRANSPARENT, self.world_bg_color)

        input_img_file_name = os.path.split(input_img.filename)[1]
        self.world_map_number = re.search(r"^wm_(\d+)", input_img_file_name).group(1)

        if self.world_map_number == None:
            raise Exception("Invalid world map name.")

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
        encode_file.write(
            "#NodeId, ContentPath (relative to Root), TopNodeId, LeftNodeId, DownNodeId, RightNodeId, Position (X, Y)\n")
        encode_file.write("[NODES]\n")

    def _encode_nodes(self, start_line, encode_file):
        serialized_nodes = []
        node_id_by_position = {}
        for y in range(start_line, start_line + self.layer_tiles_size[1]):
            for x in range(self.input_img_tiles_size[0]):
                code = self.node_identifier.get_tile_code((x, y))
                if code == IdentifierCode.CODE_NOTFOUND:
                    self._write_to_mistake_file((x, y), "node")

                elif code == IdentifierCode.CODE_EMPTY or code == IdentifierCode.CODE_VOID:
                    continue

                else:
                    node_type, index = code
                    if node_type not in [NodeType.PATH, NodeType.LOCK]:
                        sn = self._to_serialized_node(node_type, index, (x, y), start_line, node_id_by_position)
                        serialized_nodes.append(sn)

        node_ids_str = ', '.join(node_id_by_position.values())
        encode_file.write(f"{node_ids_str}")
        for sn in serialized_nodes:
            encode_file.write(f"\n{sn}")

    def _to_serialized_node(self, node_type, index, position_in_tile, start_line, node_id_by_position):

        def create_node_id_if_not_exist(node_type, index, position_in_tile):
            if position_in_tile not in node_id_by_position:
                node_id_by_position[position_in_tile] = to_node_id(node_type, index, position_in_tile)

        def to_node_id(node_type, index, position_in_tile):
            if node_type == NodeType.LEVEL:
                return f"NLevel{index + 1}{position_in_tile}"

            elif node_type == NodeType.STOP:
                return f"NStop{position_in_tile}"

            elif node_type == NodeType.START:
                return "NStart"

        def find_neighbor_node_id(position_in_tile):
            code = self.node_identifier.get_tile_code(position_in_tile)
            if code in [IdentifierCode.CODE_EMPTY, IdentifierCode.CODE_VOID, IdentifierCode.CODE_NOTFOUND]:
                return "None"

            node_type, index = code
            if node_type == NodeType.PATH:
                return "NPath"

            if node_type == NodeType.LOCK:
                return "None"

            create_node_id_if_not_exist(node_type, index, position_in_tile)
            return node_id_by_position[position_in_tile]

        def get_top(y):
            while y > start_line:
                y -= 1
                top_node_id = find_neighbor_node_id((x, y))
                if top_node_id != "NPath":
                    return top_node_id

        def get_left(x):
            while x > 0:
                x -= 1
                left_node_id = find_neighbor_node_id((x, y))
                if left_node_id != "NPath":
                    return left_node_id

        def get_bottom(y):
            while y < self.input_img_tiles_size[1]:
                y += 1
                bottom_node_id = find_neighbor_node_id((x, y))
                if bottom_node_id != "NPath":
                    return bottom_node_id

        def get_right(x):
            while x < self.input_img_tiles_size[0]:
                x += 1
                right_node_id = find_neighbor_node_id((x, y))
                if right_node_id != "NPath":
                    return right_node_id

        create_node_id_if_not_exist(node_type, index, position_in_tile)
        node_id = node_id_by_position[position_in_tile]
        x, y = position_in_tile
        # top
        top_node_id = get_top(y)
        left_node_id = get_left(x)
        bottom_node_id = get_bottom(y)
        right_node_id = get_right(x)

        content_path = "None"
        if node_type == NodeType.LEVEL:
            content_path = f"worlds/w_{self.world_map_number}_{index + 1}_1.txt"

        return f"{node_id}, {content_path}, {top_node_id}, {left_node_id}, {bottom_node_id}, {right_node_id}, {x*16}, {y*16}"


class NodeIdentifier(Identifier):
    def __init__(self, tile_anno_img, target_img_pixels, anno_img_transparent_color, target_img_bg_color):
        super().__init__(tile_anno_img, target_img_pixels, anno_img_transparent_color, target_img_bg_color)

    def _convertToCode(self, position_in_tile) -> str:
        x, y = position_in_tile
        return (NodeType(y), x)
