const int TOTAL_LEVELS = 2;
const int MAX_BRICKS = 40;
const int MAX_TOKENS = 5;
const int MAX_GHOSTS = 5;

typedef struct
{
  byte totalBricks;
  byte totalTokens;
  byte totalGhosts;
  byte bricks[MAX_BRICKS];
  byte tokens[MAX_TOKENS];
  byte ghosts[MAX_GHOSTS];
  byte goal;
  byte pacPosition;
  byte gatePosition;
  byte gateEnabled;
} Level;

// all of the game levels
volatile Level levels[TOTAL_LEVELS] = {

    {20, 4, 3, {6, 12, 24, 26, 27, 28, 31, 33, 35, 37, 39, 44, 48, 57, 59, 77, 78, 79, 64, 68}, {0, 7, 32, 65}, {40, 66, 52}, 58, 23, 38, 1},
    {32, 3, 1, {48, 68, 51, 31, 11, 19, 18, 17, 16, 15, 14, 13, 12, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 39, 59, 79, 78, 77, 76, 57, 56}, {64, 28, 71}, {32}, 58, 41, 36, 1},
};
