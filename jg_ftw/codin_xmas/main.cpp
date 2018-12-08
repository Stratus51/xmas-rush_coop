#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Help the Christmas elves fetch presents in a magical labyrinth!
 **/
void pushmaster()
{
        cout << "PUSH " << 0 << " " << "UP" << endl;

}

void movemaster()
{
        cout << "MOVE " << "LEFT " << "UP " << "RIGHT " << endl;
}

void inputTransformer()
{

}

struct Tile
{
    int x;
    int y;

    bool right;
    bool left;
    bool up;
    bool down;
};

Tile tileParser(string uTile )
{


}
int main()
{
    vector<Tile> table;

    // game loop
    while (1) {
        int turnType;
        cin >> turnType; cin.ignore();
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 7; j++) {
                Tile newtile;

                string newtile;
                cin >> newtile; cin.ignore();

                table.push_back();
            }
        }
        for (int i = 0; i < 2; i++) {
            int numPlayerCards; // the total number of quests for a player (hidden and revealed)
            int playerX;
            int playerY;
            string playerTile;
            cin >> numPlayerCards >> playerX >> playerY >> playerTile; cin.ignore();
        }
        int numItems; // the total number of items available on board and on player tiles
        cin >> numItems; cin.ignore();
        for (int i = 0; i < numItems; i++) {
            string itemName;
            int itemX;
            int itemY;
            int itemPlayerId;
            cin >> itemName >> itemX >> itemY >> itemPlayerId; cin.ignore();
        }
        int numQuests; // the total number of revealed quests for both players
        cin >> numQuests; cin.ignore();
        for (int i = 0; i < numQuests; i++) {
            string questItemName;
            int questPlayerId;
            cin >> questItemName >> questPlayerId; cin.ignore();
        }

        if (turnType == 0)
            pushmaster();
        else
            movemaster();



        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        // cout << "PUSH 3 RIGHT" << endl; // PUSH <id> <direction> | MOVE <direction> | PASS
    }
}
