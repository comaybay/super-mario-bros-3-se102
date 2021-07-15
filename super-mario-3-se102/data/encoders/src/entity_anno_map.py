from .shared_logic import EntityCode

world_entity_anno_map = {
    (0, 0): EntityCode.QUESTION_BLOCK_COIN,
    (1, 0): EntityCode.QUESTION_BLOCK_SUPER_LEAF,
    (0, 1): EntityCode.BRICK,
    (1, 1): EntityCode.BRICK_P_SWITCH,
    (2, 1): EntityCode.BRICK_1UP,
    (3, 1): EntityCode.BRICK_SUPER_LEAF,
    (4, 1): EntityCode.COIN_BLOCK_BRICK,
    (0, 2): EntityCode.COIN,
    (1, 2): EntityCode.GOAL_ROULETTE,
    (2, 2): EntityCode.PORTAL_1,
    (3, 2): EntityCode.PORTAL_2,
    (0, 3): EntityCode.GOOMBA,
    (1, 3): EntityCode.GOOMBA_RED,
    (2, 3): EntityCode.PARA_GOOMBA,
    (3, 3): EntityCode.PARA_GOOMBA_RED,
    (0, 4): EntityCode.KOOPA,
    (1, 4): EntityCode.PARA_KOOPA,
    (2, 4): EntityCode.KOOPA_RED,
    (3, 4): EntityCode.KOOPA_RED_AND_COIN,
    (0, 5): EntityCode.VENUS_RED,
    (1, 5): EntityCode.VENUS_GREEN,
    (2, 5): EntityCode.PIRANHA_GREEN,
    (0, 6): EntityCode.MARIO,
    (0, 6): EntityCode.MARIO,
    (1, 6): EntityCode.NOTE_BLOCK,
    (2, 6): EntityCode.SUPER_NOTE_BLOCK,
    (3, 6): EntityCode.ITEM_WOOD_BLOCK_SUPER_LEAF,
    (0, 7): EntityCode.BOOMERANG_BRO,
}

world_map_entity_anno_map = {
    (0, 0): EntityCode.WM_HELP_BUBBLE,
    (1, 0): EntityCode.WM_BUSH
}
