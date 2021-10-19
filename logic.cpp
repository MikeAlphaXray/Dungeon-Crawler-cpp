#include "logic.h"

#define INFO(X) cout << "[INFO] ("<<__FUNCTION__<<":"<<__LINE__<<") " << #X << " = " << X << endl;
#define INFO_STRUCT(X) cout << "[INFO] ("<<__FUNCTION__<<":"<<__LINE__<<") " << #X << " = {" << X.row << "," << X.col << "," << X.treasure << "}" << endl;

/**
 * This function is provided; do not change
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  2D dynamic array representation of dungeon map with player's location.
 */
char** loadLevel(const string fileName, int& maxRow, int& maxCol, Player& player) {
/*     INFO(maxRow);
    INFO(maxCol); */

    ifstream ifs(fileName);
    if (!ifs.is_open()) {
        cout << "ERROR: Unable to open: " << fileName << endl;
        return nullptr;
    }
    ifs >> maxRow >> maxCol;
    ifs >> player.row >> player.col;

    char** dungeon = createMap(maxRow,maxCol);
    if (dungeon == nullptr) {
        cout << "ERROR: Map did not load." << endl;
        return nullptr;
    }

    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            ifs >> dungeon[i][j];
            if (i == player.row && j == player.col) {
                dungeon[i][j] = TILE_PLAYER;
            }
        }
    }

    return dungeon;
}

/**
 * This function is provided; do not change
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    switch(input) {
        case MOVE_UP:
            nextRow--;
            break;
        case MOVE_DOWN:
            nextRow++;
            break;
        case MOVE_LEFT:
            nextCol--;
            break;
        case MOVE_RIGHT:
            nextCol++;
            break;
    }
}

/**
 * TODO: Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
/*     INFO(maxRow);
    INFO(maxCol); */

    char **map = new char*[maxRow];

    for(int i = 0; i < maxRow; ++i){
        map[i] = new char[maxCol];
        for(int j = 0; j < maxCol; j++){
            map[i][j] = TILE_OPEN;
        }
    }
    
    return map;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 */
void deleteMap(char** map, int maxRow) {
/*     INFO(map);
    INFO(maxRow); */

    for(int i = 0; i < maxRow; ++i){
        delete[] map[i];
    }

    delete[] map;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can call your createMap and deleteMap functions to help with this one!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  An updated 2D map that has twice as many columns and rows in size.
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
/*     INFO(map);
    INFO(maxRow);
    INFO(maxCol); */

    char **newMap = new char*[maxRow*2];

    for(int i = 0; i < maxRow*2; i++){
        newMap[i] = new char[maxCol*2];
        if(i < maxRow){
            for(int j = 0; j < maxCol; j++){
                newMap[i][j] = map[i][j];
            }
        }
    }

    for(int i = 0; i < maxRow; i++){
        for(int j = 0; j < maxCol; j++){
            if(map[i][j] == TILE_PLAYER){
                newMap[i+maxRow][j+maxCol] = TILE_OPEN;
                newMap[i][j+maxCol] = TILE_OPEN;
                newMap[i+maxRow][j] = TILE_OPEN;
            }else{
                newMap[i+maxRow][j+maxCol] = map[i][j];
                newMap[i][j+maxCol] = map[i][j];
                newMap[i+maxRow][j] = map[i][j];
            }
        }
    }

/*     for(int i = 0; i < maxRow*2; i++){
        for(int j = 0; j < maxCol*2; j++){
            if(i >= maxRow){
                if(map[i-maxRow][j] == TILE_PLAYER){
                    newMap[i][j] = TILE_OPEN;
                }else{
                    newMap[i][j] = map[i-maxRow][j];
                }
            }
            if(j >= maxCol){
                if(map[i][j-maxCol] == TILE_PLAYER){
                    newMap[i][j] = TILE_OPEN;
                }else{
                    newMap[i][j] = map[i][j-maxCol];
                }
            }
        }
    } */

    
    deleteMap(map, maxRow);

    maxRow *= 2;
    maxCol *= 2;
    
    return newMap;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int& nextRow, int& nextCol) {
/*     INFO(map);
    INFO(maxRow);
    INFO(maxCol);
    INFO_STRUCT(player);
    INFO(nextRow);
    INFO(nextCol); */

    if(nextCol >= maxCol || nextCol < 0){
        nextRow = player.row;
        nextCol = player.col;
        return 0;
    }
    if(nextRow >= maxRow || nextRow < 0){
        nextRow = player.row;
        nextCol = player.col;
        return 0;
    }
    if(map[nextRow][nextCol] == TILE_PILLAR || map[nextRow][nextCol] == TILE_MONSTER){
        nextRow = player.row;
        nextCol = player.col;
        return 0;
    }
    if(map[nextRow][nextCol] == TILE_TREASURE){
        player.treasure++;
        map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;

        player.row = nextRow;
        player.col = nextCol;
        return 2;
    }
    if(map[nextRow][nextCol] == TILE_AMULET){
        map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;

        player.row = nextRow;
        player.col = nextCol;
        return 3;
    }
    if(map[nextRow][nextCol] == TILE_DOOR){
        map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;

        player.row = nextRow;
        player.col = nextCol;
        return 4;
    }
    if(map[nextRow][nextCol] == TILE_EXIT){
        if(player.treasure >= 1){
            map[player.row][player.col] = TILE_OPEN;
            map[nextRow][nextCol] = TILE_PLAYER;

            player.row = nextRow;
            player.col = nextCol;
            return 5;
        }
        nextRow = player.row;
        nextCol = player.col;
        return 0;
    }
    
    map[player.row][player.col] = TILE_OPEN;
    map[nextRow][nextCol] = TILE_PLAYER;

    player.row = nextRow;
    player.col = nextCol;

    return 1; // update to use STATUS consts from logic.h!
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, Player& player) {
/*     INFO(map);
    INFO(maxRow);
    INFO(maxCol);
    INFO_STRUCT(player); */
    bool playerDead = false;
    
    // check up
    for(int i = player.row-1; i >= 0; i--){
        if(map[i][player.col] == TILE_PILLAR){
            break;
        }
        if(map[i][player.col] == TILE_MONSTER){
            if(map[i+1][player.col] == TILE_PLAYER){
                map[i][player.col] = TILE_OPEN;
                map[i+1][player.col] = TILE_MONSTER;
                playerDead = true;
            }
            map[i][player.col] = TILE_OPEN;
            map[i+1][player.col] = TILE_MONSTER;
        }
    }
    // check down
    for(int i = player.row+1; i < maxRow; i++){
        if(map[i][player.col] == TILE_PILLAR){
            break;
        }
        if(map[i][player.col] == TILE_MONSTER){
            if(map[i-1][player.col] == TILE_PLAYER){
                map[i][player.col] = TILE_OPEN;
                map[i-1][player.col] = TILE_MONSTER;
                playerDead = true;
            }
            map[i][player.col] = TILE_OPEN;
            map[i-1][player.col] = TILE_MONSTER;
        }
    }
    // check left
    for(int i = player.col-1; i >= 0; i--){
        if(map[player.row][i] == TILE_PILLAR){
            break;
        }
        if(map[player.row][i] == TILE_MONSTER){
            if(map[player.row][i+1] == TILE_PLAYER){
                map[player.row][i] = TILE_OPEN;
                map[player.row][i+1] = TILE_MONSTER;
                playerDead = true;
            }
            map[player.row][i] = TILE_OPEN;
            map[player.row][i+1] = TILE_MONSTER;
        }
    }
    // check right
    for(int i = player.col+1; i < maxCol; i++){
        if(map[player.row][i] == TILE_PILLAR){
            break;
        }
        if(map[player.row][i] == TILE_MONSTER){
            if(map[player.row][i-1] == TILE_PLAYER){
                map[player.row][i] = TILE_OPEN;
                map[player.row][i-1] = TILE_MONSTER;
                playerDead = true;
            }
            map[player.row][i] = TILE_OPEN;
            map[player.row][i-1] = TILE_MONSTER;
        }
    }

    // check if any monster is on player
    
    return playerDead;
}
