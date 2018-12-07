#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UP    0
#define RIGHT 1
#define DOWN  2
#define LEFT  3

int main()
{
	while (1) {
		int turnType;
		scanf("%d", &turnType);

		int tiles[7][7][4];
		for (int r = 0; r < 7; r++) {
			for (int c = 0; c < 7; c++) {
				char tile[5];
				scanf("%s", tile);

				for (int i = 0; i < 4; i++) {
					tiles[c][r][i] = (int)(tile[i]) - 48;
				}
			}
		}

		int numPlayerCards; // the total number of quests for a player (hidden and revealed)
		int playerX;
		int playerY;
		char playerTileChar[5];
		scanf("%d%d%d%s", &numPlayerCards, &playerX, &playerY, playerTileChar);

		int playerTile[4];
		for (int i = 0; i < 4; i++) {
			playerTile[i] = (int)(playerTileChar[i]) - 48;
		}

		int numOpponentCards; // the total number of quests for a opponent (hidden and revealed)
		int opponentX;
		int opponentY;
		char opponentTileChar[5];
		scanf("%d%d%d%s", &numOpponentCards, &opponentX, &opponentY, opponentTileChar);

		int numItems; // the total number of items available on board and on player tiles
		scanf("%d", &numItems);

		int playerItemX;
		int playerItemY;

		for (int i = 0; i < numItems; i++) {
			char itemName[11];
			int itemX;
			int itemY;
			int itemPlayerId;
			scanf("%s%d%d%d", itemName, &itemX, &itemY, &itemPlayerId);

			if (itemPlayerId == 0) {
				playerItemX = itemX;
				playerItemY = itemY;
			}
		}

		int numQuests; // the total number of revealed quests for both players
		scanf("%d", &numQuests);

		for (int i = 0; i < numQuests; i++) {
			char questItemName[11];
			int questPlayerId;
			scanf("%s%d", questItemName, &questPlayerId);
		}

		// Write an action using printf(). DON'T FORGET THE TRAILING \n
		// To debug: fprintf(stderr, "Debug messages...\n");

		if (turnType == 0) {
			printf("PUSH 3 RIGHT\n");
		} else {
			fprintf(stderr, "PX: %d, PY: %d\n", playerX, playerY);
			fprintf(stderr, "UP: %d, RIGHT: %d, DOWN: %d, LEFT: %d\n", tiles[playerX][playerY][UP], tiles[playerX][playerY][RIGHT], tiles[playerX][playerY][DOWN], tiles[playerX][playerY][LEFT]);
			fprintf(stderr, "UP: %d, RIGHT: %d, DOWN: %d, LEFT: %d\n", tiles[playerX][playerY - 1][DOWN], tiles[playerX + 1][playerY][LEFT], tiles[playerX][playerY + 1][UP], tiles[playerX - 1][playerY][RIGHT]);

			int continueToMove = 1;
			int moveNumber = 0;
			int moveList[20];

			//while (continueToMove == 1 && moveNumber < 20) {
				int moveDirectionX = playerX - playerItemX;
				int moveDirectionY = playerY - playerItemY;

				fprintf(stderr, "DX: %d, DY: %d\n", moveDirectionX, moveDirectionY);

				if (moveDirectionX < 0 // Go right
				    && tiles[playerX][playerY][RIGHT] == 1
				    && playerX < 6
				    && tiles[playerX + 1][playerY][LEFT] == 1) {
					moveList[moveNumber] = RIGHT;
					moveNumber++;
				}
				else if (moveDirectionX > 0 // Go left
				         && tiles[playerX][playerY][LEFT] == 1
				         && playerX > 0
				         && tiles[playerX - 1][playerY][RIGHT] == 1) {
					moveList[moveNumber] = LEFT;
					moveNumber++;
				}
				else if (moveDirectionY < 0 // Go down
				         && tiles[playerX][playerY][DOWN] == 1
				         && playerY < 6
				         && tiles[playerX][playerY + 1][UP] == 1) {
					moveList[moveNumber] = DOWN;
					moveNumber++;
				}
				else if (moveDirectionY > 0 // Go up
				         && tiles[playerX][playerY][UP] == 1
				         && playerY > 0
				         && tiles[playerX][playerY - 1][DOWN] == 1) {
					moveList[moveNumber] = UP;
					moveNumber++;
				}
				//if (tiles[playerX][playerY][UP] == 1
				//    && playerY > 0
				//    && tiles[playerX][playerY - 1][DOWN] == 1) {
				//	moveList[moveNumber] = UP;
				//	moveNumber++;
				//}
				//else if (tiles[playerX][playerY][RIGHT] == 1
				//         && playerX < 6
				//         && tiles[playerX + 1][playerY][LEFT] == 1) {
				//	moveList[moveNumber] = RIGHT;
				//	moveNumber++;
				//}
				//else if (tiles[playerX][playerY][DOWN] == 1
				//         && playerY < 6
				//         && tiles[playerX][playerY + 1][UP] == 1) {
				//	moveList[moveNumber] = DOWN;
				//	moveNumber++;
				//}
				//else if (tiles[playerX][playerY][LEFT] == 1
				//         && playerX > 0
				//         && tiles[playerX - 1][playerY][RIGHT] == 1) {
				//	moveList[moveNumber] = LEFT;
				//	moveNumber++;
				//}
				else {
					continueToMove = 0;
				}
			//}

			if (moveNumber > 0) {
				printf("MOVE");

				for (int i = 0; i < moveNumber; i++) {
					switch (moveList[i]) {
						case UP   : printf(" UP"); break;
						case RIGHT: printf(" RIGHT"); break;
						case DOWN : printf(" DOWN"); break;
						case LEFT : printf(" LEFT"); break;
					}
				}

				printf("\n");
			}
			else {
				printf("PASS\n");
			}
		}
	}

	return 0;
}
