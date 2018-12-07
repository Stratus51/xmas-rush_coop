import sys
import math

while True:
    turn_type = int(input())

    for i in range(7):
        for tile in input().split():
            pass

    # Player
    # num_player_cards: the total number of quests for a player (hidden and revealed)
    num_player_cards, player_x, player_y, player_tile = input().split()
    num_player_cards = int(num_player_cards)
    player_x = int(player_x)
    player_y = int(player_y)
    player_tile_up, player_tile_right, player_tile_down, player_tile_left = player_tile
    player_tile_up = int(player_tile_up)
    player_tile_right = int(player_tile_right)
    player_tile_down = int(player_tile_down)
    player_tile_left = int(player_tile_left)

    # Opponent
    # num_player_cards: the total number of quests for a player (hidden and revealed)
    num_opponent_cards, opponent_x, opponent_y, opponent_tile = input().split()
    num_opponent_cards = int(num_opponent_cards)
    opponent_x = int(opponent_x)
    opponent_y = int(opponent_y)

    num_items = int(input())  # the total number of items available on board and on player tiles

    for i in range(num_items):
        item_name, item_x, item_y, item_player_id = input().split()
        item_x = int(item_x)
        item_y = int(item_y)
        item_player_id = int(item_player_id)

    num_quests = int(input())  # the total number of revealed quests for both players

    for i in range(num_quests):
        quest_item_name, quest_player_id = input().split()
        quest_player_id = int(quest_player_id)

    # Write an action using print
    # To debug: print("Debug messages...", file=sys.stderr)

    # PUSH <id> <direction> | MOVE <direction> | PASS
    if turn_type == 0:
        print("PUSH 3 RIGHT")
    else:
        if player_tile_up == 1:
            print("MOVE UP")
        elif player_tile_right == 1:
            print("MOVE RIGHT")
        elif player_tile_down == 1:
            print("MOVE DOWN")
        elif player_tile_left == 1:
            print("MOVE LEFT")
