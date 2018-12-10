#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/***************************************************************************************************
 * Defines
 **************************************************************************************************/
#define MAX_ITEMS  12
#define MAX_QUESTS 3

#define UP    0
#define RIGHT 1
#define DOWN  2
#define LEFT  3

#define ID        0
#define DIRECTION 1

/***************************************************************************************************
 * Global types
 **************************************************************************************************/
typedef struct
{
	int x;
	int y;
} Position;

typedef struct
{
	char name[11];
	int x;
	int y;
} Item;

typedef struct
{
	char name[11];
} Quest;

typedef struct
{
	int type;
	int tiles[7][7][4];
	Position playerPosition;
	int numItems;
	Item items[MAX_ITEMS];
	int numQuests;
	Quest quests[MAX_QUESTS];
} Turn;

/***************************************************************************************************
 * Global variables
 **************************************************************************************************/
int isLastMoveTurnPass = 0;
int lastPush[2];

/***************************************************************************************************
 * Functions
 **************************************************************************************************/
Turn parseInput(void);

void pushTurn(Turn turn);
void moveTurn(Turn turn);

int canGoUp(Turn turn);
int canGoRight(Turn turn);
int canGoDown(Turn turn);
int canGoLeft(Turn turn);

/***************************************************************************************************
 * Main
 **************************************************************************************************/
int main()
{
	while (1) {
		// Create the structure that will hold all the informations of the current turn
		Turn turn = parseInput();

		// To debug: fprintf(stderr, "Debug messages...\n");

		if (turn.type == 0) {
			pushTurn(turn);
		}
		else {
			moveTurn(turn);
		}
	}

	return 0;
}

/***************************************************************************************************
 * parseInput
 **************************************************************************************************/
Turn parseInput(void)
{
	Turn turn;

	scanf("%d", &turn.type);

	for (int r = 0; r < 7; r++) {
		for (int c = 0; c < 7; c++) {
			char tile[5];
			scanf("%s", tile);

			for (int i = 0; i < 4; i++) {
				turn.tiles[c][r][i] = (int)(tile[i]) - 48;
			}
		}
	}

	int numPlayerCards; // the total number of quests for a player (hidden and revealed)
	char playerTileChar[5];
	scanf("%d%d%d%s", &numPlayerCards, &turn.playerPosition.x, &turn.playerPosition.y, playerTileChar);

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

	turn.numItems = 0;
	for (int i = 0; i < numItems; i++) {
		char itemName[11];
		int itemX;
		int itemY;
		int itemPlayerId;
		scanf("%s%d%d%d", itemName, &itemX, &itemY, &itemPlayerId);

		if (itemPlayerId == 0) {
			// Only record player items for now
			strncpy(turn.items[turn.numItems].name, itemName, 11);
			turn.items[turn.numItems].x = itemX;
			turn.items[turn.numItems].y = itemY;
			turn.numItems++;
		}
	}

	int numQuests; // the total number of revealed quests for both players
	scanf("%d", &numQuests);

	turn.numQuests = 0;
	for (int i = 0; i < numQuests; i++) {
		char questItemName[11];
		int questPlayerId;
		scanf("%s%d", questItemName, &questPlayerId);

		if (questPlayerId == 0) {
			// Only record player quests for now
			strncpy(turn.quests[turn.numQuests].name, questItemName, 11);
			turn.numQuests++;
		}
	}

	return turn;
}

/***************************************************************************************************
 * pushTurn
 **************************************************************************************************/
void pushTurn(Turn turn)
{
	if (isLastMoveTurnPass) {
		// Retry last push
		switch (lastPush[DIRECTION]) {
			case UP   : printf("PUSH %d UP\n", lastPush[ID]); break;
			case RIGHT: printf("PUSH %d RIGHT\n", lastPush[ID]); break;
			case DOWN : printf("PUSH %d DOWN\n", lastPush[ID]); break;
			case LEFT : printf("PUSH %d LEFT\n", lastPush[ID]); break;
		}
	}
	else {
		// Select the item we want to reach
		int currentItem = 0;
		for (int i = 0; i < turn.numItems; i++) {
			if (strncmp(turn.items[i].name, turn.quests[0].name, 11) == 0) {
				currentItem = i;
				break;
			}
		}

		int moveDirectionX = turn.playerPosition.x - turn.items[currentItem].x;
		int moveDirectionY = turn.playerPosition.y - turn.items[currentItem].y;

		fprintf(stderr, "DX: %d, DY: %d\n", moveDirectionX, moveDirectionY);

		if (moveDirectionY == 0) {
			if (turn.playerPosition.y > 3) {
				printf("PUSH %d UP\n", turn.playerPosition.x);
				lastPush[ID] = turn.playerPosition.x;
				lastPush[DIRECTION] = UP;
			}
			else {
				printf("PUSH %d DOWN\n", turn.playerPosition.x);
				lastPush[ID] = turn.playerPosition.x;
				lastPush[DIRECTION] = DOWN;
			}
		}
		else if (moveDirectionX == 0){
			if (turn.playerPosition.x > 3) {
				printf("PUSH %d LEFT\n", turn.playerPosition.y);
				lastPush[ID] = turn.playerPosition.y;
				lastPush[DIRECTION] = LEFT;
			}
			else {
				printf("PUSH %d RIGHT\n", turn.playerPosition.y);
				lastPush[ID] = turn.playerPosition.y;
				lastPush[DIRECTION] = RIGHT;
			}
		}
		else {
			if (moveDirectionX >= moveDirectionY) {
				if (moveDirectionX < 0) {
					printf("PUSH %d RIGHT\n", turn.playerPosition.y);
					lastPush[ID] = turn.playerPosition.y;
					lastPush[DIRECTION] = RIGHT;
				}
				else {
					printf("PUSH %d LEFT\n", turn.playerPosition.y);
					lastPush[ID] = turn.playerPosition.y;
					lastPush[DIRECTION] = LEFT;
				}
			}
			else {
				if (moveDirectionY < 0) {
					printf("PUSH %d DOWN\n", turn.playerPosition.x);
					lastPush[ID] = turn.playerPosition.x;
					lastPush[DIRECTION] = DOWN;
				}
				else {
					printf("PUSH %d UP\n", turn.playerPosition.x);
					lastPush[ID] = turn.playerPosition.x;
					lastPush[DIRECTION] = UP;
				}
			}
		}
	}
}

/***************************************************************************************************
 * moveTurn
 **************************************************************************************************/
void moveTurn(Turn turn)
{
	for (int i = 0; i < turn.numQuests; i++) {
		fprintf(stderr, "Quest: %s\n", turn.quests[i].name);
	}
	for (int i = 0; i < turn.numItems; i++) {
		fprintf(stderr, "Item: %s\n", turn.items[i].name);
	}

	// Select the item we want to reach
	int currentItem = 0;
	for (int i = 0; i < turn.numItems; i++) {
		if (strncmp(turn.items[i].name, turn.quests[0].name, 11) == 0) {
			currentItem = i;
			break;
		}
	}
	fprintf(stderr, "Selected item: %s\n", turn.items[currentItem].name);

	// Try to go to the selected item
	int moveNumber = 0;
	int moveList[20];
	int continueToMove = 1;

	while (continueToMove && moveNumber < 20) {
		int moveDirectionX = turn.playerPosition.x - turn.items[currentItem].x;
		int moveDirectionY = turn.playerPosition.y - turn.items[currentItem].y;

		fprintf(stderr, "PX: %d, PY: %d\n", turn.playerPosition.x, turn.playerPosition.y);
		fprintf(stderr, "UP: %d, RIGHT: %d, DOWN: %d, LEFT: %d\n", turn.tiles[turn.playerPosition.x][turn.playerPosition.y][UP], turn.tiles[turn.playerPosition.x][turn.playerPosition.y][RIGHT], turn.tiles[turn.playerPosition.x][turn.playerPosition.y][DOWN], turn.tiles[turn.playerPosition.x][turn.playerPosition.y][LEFT]);
		fprintf(stderr, "UP: %d, RIGHT: %d, DOWN: %d, LEFT: %d\n", turn.tiles[turn.playerPosition.x][turn.playerPosition.y - 1][DOWN], turn.tiles[turn.playerPosition.x + 1][turn.playerPosition.y][LEFT], turn.tiles[turn.playerPosition.x][turn.playerPosition.y + 1][UP], turn.tiles[turn.playerPosition.x - 1][turn.playerPosition.y][RIGHT]);
		fprintf(stderr, "DX: %d, DY: %d\n", moveDirectionX, moveDirectionY);
		fprintf(stderr, "Can go UP: %d, RIGHT: %d, DOWN: %d, LEFT: %d\n", canGoUp(turn), canGoRight(turn), canGoDown(turn), canGoLeft(turn));

		if (moveDirectionX < 0 && canGoRight(turn)) {
			moveList[moveNumber] = RIGHT;
			moveNumber++;

			turn.playerPosition.x++;
		}
		else if (moveDirectionX > 0 && canGoLeft(turn)) {
			moveList[moveNumber] = LEFT;
			moveNumber++;

			turn.playerPosition.x--;
		}
		else if (moveDirectionY < 0 && canGoDown(turn)) {
			moveList[moveNumber] = DOWN;
			moveNumber++;

			turn.playerPosition.y++;
		}
		else if (moveDirectionY > 0 && canGoUp(turn)) {
			moveList[moveNumber] = UP;
			moveNumber++;

			turn.playerPosition.y--;
		}
		else {
			continueToMove = 0;
		}
	}

	if (moveNumber == 0) {
		continueToMove = 1;

		while (continueToMove && moveNumber < 20) {
			int lastMove;
			if (canGoRight(turn) && lastMove != LEFT) {
				moveList[moveNumber] = RIGHT;
				moveNumber++;
				lastMove = RIGHT;

				turn.playerPosition.x++;
			}
			else if (canGoLeft(turn) && lastMove != RIGHT) {
				moveList[moveNumber] = LEFT;
				moveNumber++;
				lastMove = LEFT;

				turn.playerPosition.x--;
			}
			else if (canGoDown(turn) && lastMove != UP) {
				moveList[moveNumber] = DOWN;
				moveNumber++;
				lastMove = DOWN;

				turn.playerPosition.y++;
			}
			else if (canGoUp(turn) && lastMove != DOWN) {
				moveList[moveNumber] = UP;
				moveNumber++;
				lastMove = UP;

				turn.playerPosition.y--;
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

		isLastMoveTurnPass = 0;
	}
	else {
		printf("PASS\n");

		isLastMoveTurnPass = 1;
	}
}

/***************************************************************************************************
 * canGoUp
 **************************************************************************************************/
int canGoUp(Turn turn)
{
	if (turn.tiles[turn.playerPosition.x][turn.playerPosition.y][UP]
	    && turn.playerPosition.y > 0
	    && turn.tiles[turn.playerPosition.x][turn.playerPosition.y - 1][DOWN]) {
		return 1;
	}
	else {
		return 0;
	}
}

/***************************************************************************************************
 * canGoRight
 **************************************************************************************************/
int canGoRight(Turn turn)
{
	if (turn.tiles[turn.playerPosition.x][turn.playerPosition.y][RIGHT]
	    && turn.playerPosition.x < 6
	    && turn.tiles[turn.playerPosition.x + 1][turn.playerPosition.y][LEFT]) {
		return 1;
	}
	else {
		return 0;
	}
}

/***************************************************************************************************
 * canGoDown
 **************************************************************************************************/
int canGoDown(Turn turn)
{
	if (turn.tiles[turn.playerPosition.x][turn.playerPosition.y][DOWN]
	    && turn.playerPosition.y < 6
	    && turn.tiles[turn.playerPosition.x][turn.playerPosition.y + 1][UP]) {
		return 1;
	}
	else {
		return 0;
	}
}

/***************************************************************************************************
 * canGoLeft
 **************************************************************************************************/
int canGoLeft(Turn turn)
{
	if (turn.tiles[turn.playerPosition.x][turn.playerPosition.y][LEFT]
	    && turn.playerPosition.x > 0
	    && turn.tiles[turn.playerPosition.x - 1][turn.playerPosition.y][RIGHT]) {
		return 1;
	}
	else {
		return 0;
	}
}
