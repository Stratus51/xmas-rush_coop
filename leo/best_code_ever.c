#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/***************************************************************************************************
 * Defines
 **************************************************************************************************/
#define MAX_ITEMS  12
#define MAX_QUESTS 3
#define MAX_MOVES  20

#define UP    0
#define RIGHT 1
#define DOWN  2
#define LEFT  3
#define NONE  4

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

void pushTurn(const Turn turn);
void moveTurn(const Turn turn);

int canGoUp(const Turn turn);
int canGoRight(const Turn turn);
int canGoDown(const Turn turn);
int canGoLeft(const Turn turn);

int findPathToItem(const Turn turn,
                   const int currentItem,
				   const int lastMove,
				   int *pathNumber,
				   int movesNumber[MAX_MOVES],
				   int movesList[MAX_MOVES][MAX_MOVES+2],
				   Position previousPath[49]);

int notOnPreviousPath(const Position playerPosition, const Position previousPath[49]);
void addPositionToPreviousPath(const Position playerPosition, Position previousPath[49]);

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
void pushTurn(const Turn turn)
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
void moveTurn(const Turn turn)
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
	int moveList[MAX_MOVES];

	int pathNumber = 0;
	int movesNumber[MAX_MOVES] = {0};
	int movesList[MAX_MOVES][MAX_MOVES+2] = {{0}};
	Position previousPath[49];

	// Initalize the previous path position array with unreachable value to detect which index is
	// not yet used
	for (int i = 0; i < 49; i++) {
		previousPath[i].x = 10;
		previousPath[i].y = 10;
	}

	if (findPathToItem(turn, currentItem, NONE, &pathNumber, movesNumber, movesList, previousPath)) {
		moveNumber = movesNumber[pathNumber];

		fprintf(stderr, "Number of path: %d\n", pathNumber);
		fprintf(stderr, "Item found :)\nMove:");
		for (int i = 0; i < moveNumber; i++) {
			fprintf(stderr, " %d", movesList[pathNumber][i]);
			moveList[i] = movesList[pathNumber][i];
		}
	    fprintf(stderr, "\n");
	}
	else {
		// Print all paths
		fprintf(stderr, "COUCOUCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC\n");
		fprintf(stderr, "Number of path: %d\n", pathNumber);
		for (int i = 0; i < pathNumber; i++) {
			fprintf(stderr, "Path number: %d\nMove:", i);

			for (int j = 0; j < movesNumber[i]; j++) {
				fprintf(stderr, " %d", movesList[i][j]);
			}
			fprintf(stderr, "\n");
		}

		// Take the first path for the moment
	    fprintf(stderr, "Number of path: %d\n", pathNumber);

		moveNumber = movesNumber[0];

	    fprintf(stderr, "Item not found :(\nMove:");
		for (int i = 0; i < moveNumber; i++) {
			fprintf(stderr, " %d", movesList[0][i]);
			moveList[i] = movesList[0][i];
		}
	    fprintf(stderr, "\n");
	}


	//int continueToMove = 1;

	//while (continueToMove && moveNumber < 20) {
	//	int moveDirectionX = turn.playerPosition.x - turn.items[currentItem].x;
	//	int moveDirectionY = turn.playerPosition.y - turn.items[currentItem].y;

	//	fprintf(stderr, "PX: %d, PY: %d\n", turn.playerPosition.x, turn.playerPosition.y);
	//	fprintf(stderr, "UP: %d, RIGHT: %d, DOWN: %d, LEFT: %d\n", turn.tiles[turn.playerPosition.x][turn.playerPosition.y][UP], turn.tiles[turn.playerPosition.x][turn.playerPosition.y][RIGHT], turn.tiles[turn.playerPosition.x][turn.playerPosition.y][DOWN], turn.tiles[turn.playerPosition.x][turn.playerPosition.y][LEFT]);
	//	fprintf(stderr, "UP: %d, RIGHT: %d, DOWN: %d, LEFT: %d\n", turn.tiles[turn.playerPosition.x][turn.playerPosition.y - 1][DOWN], turn.tiles[turn.playerPosition.x + 1][turn.playerPosition.y][LEFT], turn.tiles[turn.playerPosition.x][turn.playerPosition.y + 1][UP], turn.tiles[turn.playerPosition.x - 1][turn.playerPosition.y][RIGHT]);
	//	fprintf(stderr, "DX: %d, DY: %d\n", moveDirectionX, moveDirectionY);
	//	fprintf(stderr, "Can go UP: %d, RIGHT: %d, DOWN: %d, LEFT: %d\n", canGoUp(turn), canGoRight(turn), canGoDown(turn), canGoLeft(turn));

	//	if (moveDirectionX < 0 && canGoRight(turn)) {
	//		moveList[moveNumber] = RIGHT;
	//		moveNumber++;

	//		turn.playerPosition.x++;
	//	}
	//	else if (moveDirectionX > 0 && canGoLeft(turn)) {
	//		moveList[moveNumber] = LEFT;
	//		moveNumber++;

	//		turn.playerPosition.x--;
	//	}
	//	else if (moveDirectionY < 0 && canGoDown(turn)) {
	//		moveList[moveNumber] = DOWN;
	//		moveNumber++;

	//		turn.playerPosition.y++;
	//	}
	//	else if (moveDirectionY > 0 && canGoUp(turn)) {
	//		moveList[moveNumber] = UP;
	//		moveNumber++;

	//		turn.playerPosition.y--;
	//	}
	//	else {
	//		continueToMove = 0;
	//	}
	//}

	//if (moveNumber == 0) {
	//	continueToMove = 1;

	//	while (continueToMove && moveNumber < 20) {
	//		int lastMove;
	//		if (canGoRight(turn) && lastMove != LEFT) {
	//			moveList[moveNumber] = RIGHT;
	//			moveNumber++;
	//			lastMove = RIGHT;

	//			turn.playerPosition.x++;
	//		}
	//		else if (canGoLeft(turn) && lastMove != RIGHT) {
	//			moveList[moveNumber] = LEFT;
	//			moveNumber++;
	//			lastMove = LEFT;

	//			turn.playerPosition.x--;
	//		}
	//		else if (canGoDown(turn) && lastMove != UP) {
	//			moveList[moveNumber] = DOWN;
	//			moveNumber++;
	//			lastMove = DOWN;

	//			turn.playerPosition.y++;
	//		}
	//		else if (canGoUp(turn) && lastMove != DOWN) {
	//			moveList[moveNumber] = UP;
	//			moveNumber++;
	//			lastMove = UP;

	//			turn.playerPosition.y--;
	//		}
	//		else {
	//			continueToMove = 0;
	//		}
	//	}
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
int canGoUp(const Turn turn)
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
int canGoRight(const Turn turn)
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
int canGoDown(const Turn turn)
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
int canGoLeft(const Turn turn)
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

/***************************************************************************************************
 * findPathToItem
 **************************************************************************************************/
int findPathToItem(const Turn turn,
                   const int currentItem,
				   const int lastMove,
				   int *pathNumber,
				   int movesNumber[MAX_MOVES],
				   int movesList[MAX_MOVES][MAX_MOVES+2],
				   Position previousPath[49])
{
	int branchNumber = 0;
	fprintf(stderr, "PX: %d, PY: %d\n", turn.playerPosition.x, turn.playerPosition.y);

	// First check if we have already reach the item
	if (turn.playerPosition.x == turn.items[currentItem].x
	    && turn.playerPosition.y == turn.items[currentItem].y) {
		fprintf(stderr, "Item found !");
		return 1;
	}

	// Check that we can still move on this path
	if (movesNumber[*pathNumber] < MAX_MOVES
	    && notOnPreviousPath(turn.playerPosition, previousPath)) {
		addPositionToPreviousPath(turn.playerPosition, previousPath);

		if (canGoUp(turn) && lastMove != DOWN) {
			branchNumber++;
			fprintf(stderr, "New up move\n");
			// Try to find the item in the next up tile
			Turn nextPosition = turn;
			nextPosition.playerPosition.y--;
			movesList[*pathNumber][movesNumber[*pathNumber]] = UP;
			movesNumber[*pathNumber]++;

			if (findPathToItem(nextPosition, currentItem, UP, pathNumber, movesNumber, movesList, previousPath)) {
				return 1;
			}
		}

		if (canGoRight(turn) && lastMove != LEFT) {
			if (branchNumber > 0) {
				// Find common ancestor and add common path at the begining of this path
				Position previousPosition = {movesList[*pathNumber-1][MAX_MOVES], movesList[*pathNumber-1][MAX_MOVES+1]};
				for (int i = movesNumber[*pathNumber-1] - 1; i >= 0; i--) {
					switch (movesList[*pathNumber-1][i]) {
						case UP   : previousPosition.y++; break;
						case RIGHT: previousPosition.x--; break;
						case DOWN : previousPosition.y--; break;
						case LEFT : previousPosition.x++; break;
					}
					if (previousPosition.x == turn.playerPosition.x && previousPosition.y == turn.playerPosition.y) {
						// Common ancestor
						fprintf(stderr, "Common ancestor PX: %d, PY: %d\n", turn.playerPosition.x, turn.playerPosition.y);

						for (int j = 0; j < i; j++) {
							movesList[*pathNumber][j] = movesList[*pathNumber-1][j];
							movesNumber[*pathNumber] = j + 1;
							fprintf(stderr, " %d", movesList[*pathNumber][j]);
						}
						break;
					}
				}
			}

			branchNumber++;
			fprintf(stderr, "New right move\n");
			// Try to find the item in the next right tile
			Turn nextPosition = turn;
			nextPosition.playerPosition.x++;
			movesList[*pathNumber][movesNumber[*pathNumber]] = RIGHT;
			movesNumber[*pathNumber]++;

			if (findPathToItem(nextPosition, currentItem, RIGHT, pathNumber, movesNumber, movesList, previousPath)) {
				return 1;
			}
		}

		if (canGoDown(turn) && lastMove != UP) {
			if (branchNumber > 0) {
				// Find common ancestor and add common path at the begining of this path
				Position previousPosition = {movesList[*pathNumber-1][MAX_MOVES], movesList[*pathNumber-1][MAX_MOVES+1]};
				for (int i = movesNumber[*pathNumber-1] - 1; i >= 0; i--) {
					switch (movesList[*pathNumber-1][i]) {
						case UP   : previousPosition.y++; break;
						case RIGHT: previousPosition.x--; break;
						case DOWN : previousPosition.y--; break;
						case LEFT : previousPosition.x++; break;
					}
					if (previousPosition.x == turn.playerPosition.x && previousPosition.y == turn.playerPosition.y) {
						// Common ancestor
						fprintf(stderr, "Common ancestor PX: %d, PY: %d\n", turn.playerPosition.x, turn.playerPosition.y);

						for (int j = 0; j < i; j++) {
							movesList[*pathNumber][j] = movesList[*pathNumber-1][j];
							movesNumber[*pathNumber] = j + 1;
							fprintf(stderr, " %d", movesList[*pathNumber][j]);
						}
						break;
					}
				}
			}

			branchNumber++;
			fprintf(stderr, "New down move\n");
			// Try to find the item in the next down tile
			Turn nextPosition = turn;
			nextPosition.playerPosition.y++;
			movesList[*pathNumber][movesNumber[*pathNumber]] = DOWN;
			movesNumber[*pathNumber]++;

			if (findPathToItem(nextPosition, currentItem, DOWN, pathNumber, movesNumber, movesList, previousPath)) {
				return 1;
			}
		}

		if (canGoLeft(turn) && lastMove != RIGHT) {
			if (branchNumber > 0) {
				// Find common ancestor and add common path at the begining of this path
				Position previousPosition = {movesList[*pathNumber-1][MAX_MOVES], movesList[*pathNumber-1][MAX_MOVES+1]};
				for (int i = movesNumber[*pathNumber-1] - 1; i >= 0; i--) {
					switch (movesList[*pathNumber-1][i]) {
						case UP   : previousPosition.y++; break;
						case RIGHT: previousPosition.x--; break;
						case DOWN : previousPosition.y--; break;
						case LEFT : previousPosition.x++; break;
					}
					if (previousPosition.x == turn.playerPosition.x && previousPosition.y == turn.playerPosition.y) {
						// Common ancestor
						fprintf(stderr, "Common ancestor PX: %d, PY: %d\n", turn.playerPosition.x, turn.playerPosition.y);

						for (int j = 0; j < i; j++) {
							movesList[*pathNumber][j] = movesList[*pathNumber-1][j];
							movesNumber[*pathNumber] = j + 1;
							fprintf(stderr, " %d", movesList[*pathNumber][j]);
						}
						break;
					}
				}
			}

			branchNumber++;
			fprintf(stderr, "New left move\n");
			// Try to find the item in the next left tile
			Turn nextPosition = turn;
			nextPosition.playerPosition.x--;
			movesList[*pathNumber][movesNumber[*pathNumber]] = LEFT;
			movesNumber[*pathNumber]++;

			if (findPathToItem(nextPosition, currentItem, LEFT, pathNumber, movesNumber, movesList, previousPath)) {
				return 1;
			}
		}
	}

	fprintf(stderr, "Branch number: %d\n", branchNumber);

	if (branchNumber == 0) {
		fprintf(stderr, "End of path reached\n");
		fprintf(stderr, "Item not found on this path:");
		for (int i = 0; i < movesNumber[*pathNumber]; i++) {
			fprintf(stderr, " %d", movesList[*pathNumber][i]);
		}

		// Add the current position at the end of the path
		fprintf(stderr, "Dead end position PX: %d, PY: %d\n", turn.playerPosition.x, turn.playerPosition.y);
		movesList[*pathNumber][MAX_MOVES] = turn.playerPosition.x;
		movesList[*pathNumber][MAX_MOVES+1] = turn.playerPosition.y;

		fprintf(stderr, "\nTry new path\n");
		// We have reached the end of this path, try a new one
		*pathNumber = *pathNumber + 1;
		fprintf(stderr, "New path number: %d\n", *pathNumber);
	}

	return 0;
}

/***************************************************************************************************
 * notOnPreviousJunction
 **************************************************************************************************/
int notOnPreviousPath(const Position playerPosition, const Position previousPath[49])
{
	for (int i = 0; i < 49; i++) {
		if (playerPosition.x == previousPath[i].x && playerPosition.y == previousPath[i].y) {
			return 0;
		}
		if (previousPath[i].x == 10) {
			// End of positions
			break;
		}
	}
	return 1;
}

/***************************************************************************************************
 * addPositionToPreviousPath
 **************************************************************************************************/
void addPositionToPreviousPath(const Position playerPosition, Position previousPath[49])
{
	if (notOnPreviousPath(playerPosition, previousPath)) {
		for (int i = 0; i < 49; i++) {
			if (previousPath[i].x == 10) {
				// End of positions, add new here
				previousPath[i] = playerPosition;
				break;
			}
		}
	}
}
