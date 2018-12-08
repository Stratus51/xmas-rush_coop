#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UP    0
#define RIGHT 1
#define DOWN  2
#define LEFT  3

#define ID        0
#define DIRECTION 1

int tiles[7][7][4];

int playerX;
int playerY;

int canGoUp(void);
int canGoRight(void);
int canGoDown(void);
int canGoLeft(void);

int main()
{
	int isLastPass = 0;
	int lastPush[2];

	while (1) {
		int turnType;
		scanf("%d", &turnType);

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
			if (isLastPass) {
				// Retry last push
				switch (lastPush[DIRECTION]) {
					case UP   : printf("PUSH %d UP\n", lastPush[ID]); break;
					case RIGHT: printf("PUSH %d RIGHT\n", lastPush[ID]); break;
					case DOWN : printf("PUSH %d DOWN\n", lastPush[ID]); break;
					case LEFT : printf("PUSH %d LEFT\n", lastPush[ID]); break;
				}
			}
			else {
				int moveDirectionX = playerX - playerItemX;
				int moveDirectionY = playerY - playerItemY;

				fprintf(stderr, "DX: %d, DY: %d\n", moveDirectionX, moveDirectionY);

				if (moveDirectionY == 0) {
					if (playerY > 3) {
						printf("PUSH %d UP\n", playerX);
						lastPush[ID] = playerX;
						lastPush[DIRECTION] = UP;
					}
					else {
						printf("PUSH %d DOWN\n", playerX);
						lastPush[ID] = playerX;
						lastPush[DIRECTION] = DOWN;
					}
				}
				else if (moveDirectionX == 0){
					if (playerX > 3) {
						printf("PUSH %d LEFT\n", playerY);
						lastPush[ID] = playerY;
						lastPush[DIRECTION] = LEFT;
					}
					else {
						printf("PUSH %d RIGHT\n", playerY);
						lastPush[ID] = playerY;
						lastPush[DIRECTION] = RIGHT;
					}
				}
				else {
					if (moveDirectionX >= moveDirectionY) {
						if (moveDirectionX < 0) {
							printf("PUSH %d RIGHT\n", playerY);
							lastPush[ID] = playerY;
							lastPush[DIRECTION] = RIGHT;
						}
						else {
							printf("PUSH %d LEFT\n", playerY);
							lastPush[ID] = playerY;
							lastPush[DIRECTION] = LEFT;
						}
					}
					else {
						if (moveDirectionY < 0) {
							printf("PUSH %d DOWN\n", playerX);
							lastPush[ID] = playerX;
							lastPush[DIRECTION] = DOWN;
						}
						else {
							printf("PUSH %d UP\n", playerX);
							lastPush[ID] = playerX;
							lastPush[DIRECTION] = UP;
						}
					}
				}
			}
		}
		else {
			int moveNumber = 0;
			int moveList[20];
			int continueToMove = 1;

			while (continueToMove && moveNumber < 20) {
				int moveDirectionX = playerX - playerItemX;
				int moveDirectionY = playerY - playerItemY;

				fprintf(stderr, "PX: %d, PY: %d\n", playerX, playerY);
				fprintf(stderr, "UP: %d, RIGHT: %d, DOWN: %d, LEFT: %d\n", tiles[playerX][playerY][UP], tiles[playerX][playerY][RIGHT], tiles[playerX][playerY][DOWN], tiles[playerX][playerY][LEFT]);
				fprintf(stderr, "UP: %d, RIGHT: %d, DOWN: %d, LEFT: %d\n", tiles[playerX][playerY - 1][DOWN], tiles[playerX + 1][playerY][LEFT], tiles[playerX][playerY + 1][UP], tiles[playerX - 1][playerY][RIGHT]);
				fprintf(stderr, "DX: %d, DY: %d\n", moveDirectionX, moveDirectionY);
				fprintf(stderr, "Can go UP: %d, RIGHT: %d, DOWN: %d, LEFT: %d\n", canGoUp(), canGoRight(), canGoDown(), canGoLeft());

				if (moveDirectionX < 0 && canGoRight()) {
					moveList[moveNumber] = RIGHT;
					moveNumber++;

					playerX++;
				}
				else if (moveDirectionX > 0 && canGoLeft()) {
					moveList[moveNumber] = LEFT;
					moveNumber++;

					playerX--;
				}
				else if (moveDirectionY < 0 && canGoDown()) {
					moveList[moveNumber] = DOWN;
					moveNumber++;

					playerY++;
				}
				else if (moveDirectionY > 0 && canGoUp()) {
					moveList[moveNumber] = UP;
					moveNumber++;

					playerY--;
				}
				else {
					continueToMove = 0;
				}
			}

			if (moveNumber == 0) {
				continueToMove = 1;

				while (continueToMove && moveNumber < 20) {
					int lastMove;
					if (canGoRight() && lastMove != LEFT) {
						moveList[moveNumber] = RIGHT;
						moveNumber++;
						lastMove = RIGHT;

						playerX++;
					}
					else if (canGoLeft() && lastMove != RIGHT) {
						moveList[moveNumber] = LEFT;
						moveNumber++;
						lastMove = LEFT;

						playerX--;
					}
					else if (canGoDown() && lastMove != UP) {
						moveList[moveNumber] = DOWN;
						moveNumber++;
						lastMove = DOWN;

						playerY++;
					}
					else if (canGoUp() && lastMove != DOWN) {
						moveList[moveNumber] = UP;
						moveNumber++;
						lastMove = UP;

						playerY--;
					}
					else {
						continueToMove = 0;
					}
				}
			}

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

				isLastPass = 0;
			}
			else {
				printf("PASS\n");

				isLastPass = 1;
			}
		}
	}

	return 0;
}

int canGoUp(void)
{
	if (tiles[playerX][playerY][UP]
	    && playerY > 0
	    && tiles[playerX][playerY - 1][DOWN]) {
		return 1;
	}
	else {
		return 0;
	}
}

int canGoRight(void)
{
	if (tiles[playerX][playerY][RIGHT]
	    && playerX < 6
	    && tiles[playerX + 1][playerY][LEFT]) {
		return 1;
	}
	else {
		return 0;
	}
}

int canGoDown(void)
{
	if (tiles[playerX][playerY][DOWN]
	    && playerY < 6
	    && tiles[playerX][playerY + 1][UP]) {
		return 1;
	}
	else {
		return 0;
	}
}

int canGoLeft(void)
{
	if (tiles[playerX][playerY][LEFT]
	    && playerX > 0
	    && tiles[playerX - 1][playerY][RIGHT]) {
		return 1;
	}
	else {
		return 0;
	}
}
