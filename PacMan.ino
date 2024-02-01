// include the library code:
#include <LiquidCrystal.h>
#include "Headers.h"
#include "Levels.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(1, 0, 16, 17, 18, 19);

int motor = 2;       // D2
int horizontal = 15; // A1
int vertical = 14;   // A0

// the readings from the joystick
volatile int xVal;
volatile int yVal;

// the game level
int level = 0;
int timerCounter;
int inRunning = 0;

volatile int ghostToggle = 0;
volatile int pacToggle = 0;
volatile int tokenToggle = 0;
volatile int collectedTokens = 0;

// directio 0=up, 1=down, 2=right, 3=left
int pacDirection = 2;
int ghostMoveCnt = 0;

Level *volatile currentLevel;

void setup()
{
  // will be used to drive the haptic feedback motor
  pinMode(motor, OUTPUT);
  // register the custom chars
  setupGameCharacters();
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);

  registerInterupts();
}

void registerInterupts()
{
  // initialize timer1
  noInterrupts(); // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  timerCounter = 50000; // preload timer 65536-16MHz/256/2Hz

  TCNT1 = timerCounter;   // preload timer
  TCCR1B |= (1 << CS12);  // 256 prescaler
  TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
}

void setupGameCharacters()
{
  lcd.createChar(pacRight, pac_right);
  lcd.createChar(pacLeft, pac_left);
  lcd.createChar(pacUp, pac_up);
  lcd.createChar(pacDown, pac_down);
  lcd.createChar(pacClosed, pac_closed);
  lcd.createChar(ghost1, ghost_1);
  lcd.createChar(ghost2, ghost_2);
  lcd.createChar(token1, token_1);
  // lcd.createChar(token2, token_2);
}

void loop()
{
  startGame();
}

void startGame()
{
  lcd.clear();
  lcd.setCursor(3, 2);
  lcd.print("Level ");
  lcd.setCursor(9, 2);
  lcd.print((level + 1));
  delay(5000);

  currentLevel = (Level *volatile)&levels[level];
  collectedTokens = 0;
  buildLevel();

  inRunning = 1;
  // enabled interupts
  interrupts();

  while (inRunning)
  {
    movePac();
    delay(100);
  }
}

ISR(TIMER1_OVF_vect) // interrupt service routine
{
  if (inRunning)
  {
    flitterGhost();
    flapPac();
    spinToken();
    if (ghostMoveCnt == 3)
    {
      ghostMoveCnt = 0;
      moveGhosts();
    }
    else
    {
      ghostMoveCnt++;
    }
    TCNT1 = timerCounter;
  }
}

void flitterGhost()
{
  int i, r, c;
  ghostToggle = !ghostToggle;
  for (i = 0; i < currentLevel->totalGhosts; i++)
  {
    r = getRow(currentLevel->ghosts[i]);
    c = getCol(currentLevel->ghosts[i]);
    lcd.setCursor(c, r);

    if (ghostToggle)
    {
      lcd.write(byte(ghost1));
    }
    else
    {
      lcd.write(byte(ghost2));
    }
  }
}

void flapPac()
{
  int i, r, c;
  pacToggle = !pacToggle;

  r = getRow(currentLevel->pacPosition);
  c = getCol(currentLevel->pacPosition);
  lcd.setCursor(c, r);

  if (pacToggle)
  {
    lcd.write(byte(pacDirection));
  }
  else
  {
    lcd.write(byte(pacClosed));
  }
}

void spinToken()
{
  int i, r, c;
  tokenToggle = !tokenToggle;
  for (i = 0; i < currentLevel->totalTokens; i++)
  {
    // this token has been eaten
    if (currentLevel->tokens[i] == 255)
      continue;

    r = getRow(currentLevel->tokens[i]);
    c = getCol(currentLevel->tokens[i]);

    lcd.setCursor(c, r);

    if (tokenToggle)
    {
      lcd.write(byte(token1));
    }
    else
    {
      // lcd.write(byte(token2));
    }
  }
}

void movePac()
{
  xVal = analogRead(horizontal);
  yVal = analogRead(vertical);

  int xmag = 0;
  int ymag = 0;
  int r, c;

  xmag = (xVal < 520) ? (520 - xVal) : xVal - 520;
  ymag = (yVal < 520) ? (520 - yVal) : yVal - 520;

  if (xmag < 300 && ymag < 300)
    return;

  // we are moving horizontally
  if (xmag >= ymag)
  {
    c = getCol(currentLevel->pacPosition);
    // left
    if (xVal < 200)
    {
      // if we on the rightmost edge
      if (c == 19)
      {
        // can we wrap around
        if (isSpaceEmpty(currentLevel->pacPosition - 19))
        {
          // move him there
          clearChar(currentLevel->pacPosition);
          currentLevel->pacPosition -= 19;
          drawPac(currentLevel->pacPosition, 2);
        } // else not empty and we cannot move
      }
      else
      {
        // we are not on the right edge
        if (isSpaceEmpty(currentLevel->pacPosition + 1))
        {
          // move him there
          clearChar(currentLevel->pacPosition);
          currentLevel->pacPosition += 1;
          drawPac(currentLevel->pacPosition, 2);
        } // else not empty and we cannot move
      }
    }
    // right
    else
    {
      // if we on the leftmost edge
      if (c == 0)
      {
        // can we wrap around
        if (isSpaceEmpty(currentLevel->pacPosition + 19))
        {
          // move him there
          clearChar(currentLevel->pacPosition);
          currentLevel->pacPosition += 19;
          drawPac(currentLevel->pacPosition, 3);
        } // else not empty and we cannot move
      }
      else
      {
        // we are not on the left edge
        if (isSpaceEmpty(currentLevel->pacPosition - 1))
        {
          // move him there
          clearChar(currentLevel->pacPosition);
          currentLevel->pacPosition -= 1;
          drawPac(currentLevel->pacPosition, 3);
        } // else not empty and we cannot move
      }
    }
  }
  else
  {
    r = getRow(currentLevel->pacPosition);

    // up
    if (yVal < 200)
    {
      // if we on the top row
      if (r == 0)
      {
        // can we wrap around
        if (isSpaceEmpty(currentLevel->pacPosition + 60))
        {
          // move him there
          clearChar(currentLevel->pacPosition);
          currentLevel->pacPosition += 60;
          drawPac(currentLevel->pacPosition, 0);
        } // else not empty and we cannot move
      }
      else
      {
        // we are not on the top
        if (isSpaceEmpty(currentLevel->pacPosition - 20))
        {
          // move him there
          clearChar(currentLevel->pacPosition);
          currentLevel->pacPosition -= 20;
          drawPac(currentLevel->pacPosition, 0);
        } // else not empty and we cannot move
      }
    }
    // down
    else
    {
      // if we on the bottom row
      if (r == 3)
      {
        // can we wrap around
        if (isSpaceEmpty(currentLevel->pacPosition - 60))
        {
          // move him there
          clearChar(currentLevel->pacPosition);
          currentLevel->pacPosition -= 60;
          drawPac(currentLevel->pacPosition, 1);
        } // else not empty and we cannot move
      }
      else
      {
        // we are not on the bottom
        if (isSpaceEmpty(currentLevel->pacPosition + 20))
        {
          // move him there
          clearChar(currentLevel->pacPosition);
          currentLevel->pacPosition += 20;
          drawPac(currentLevel->pacPosition, 1);
        } // else not empty and we cannot move
      }
    }
  }
  delay(250);
  if (checkGhostHit())
  {
    gameOver();
  }

  checkHitToken();
  checkHitGoal();
}

void checkHitGoal()
{
  // level up
  if (currentLevel->goal == currentLevel->pacPosition)
  {
    level++;
    inRunning = 0;
  }
}

void checkHitToken()
{
  // first check if we have any tokens at all...
  // if we set none then this check means nothing
  if (currentLevel->totalTokens == 0)
    return;

  boolean hit = false;
  int i, rg, cg, rp, cp;
  for (i = 0; i < currentLevel->totalTokens; i++)
  {
    // we have already eaten this token
    if (currentLevel->tokens[i] == 255)
      continue;

    // if we are on the pac, gameover
    if (currentLevel->tokens[i] == currentLevel->pacPosition)
    {
      currentLevel->tokens[i] = 255;
      hit = true;
      break;
    }
  }

  if (hit)
  {
    collectedTokens++;
    if (currentLevel->totalTokens == collectedTokens)
    {
      unlockGate();
    }
  }
}

void unlockGate()
{
  currentLevel->gateEnabled = 0;
  setLCDCursor(currentLevel->gatePosition);
  lcd.print(" ");
}

void setLCDCursor(int position)
{
  int r, c;

  r = getRow(position);
  c = getCol(position);

  lcd.setCursor(c, r);
}

void clearChar(int position)
{
  setLCDCursor(position);
  lcd.print(" ");
}

// directio 0=up, 1=down, 2=right, 3=left
void drawPac(int position, int dir)
{
  setLCDCursor(position);
  pacDirection = dir;
  lcd.write(byte(pacDirection));
}

bool checkGhostHit()
{
  int i, rg, cg, rp, cp;
  for (i = 0; i < currentLevel->totalGhosts; i++)
  {
    // where is the ghost
    rg = getRow(currentLevel->ghosts[i]);
    cg = getCol(currentLevel->ghosts[i]);
    // where is the pac
    rp = getRow(currentLevel->pacPosition);
    cp = getCol(currentLevel->pacPosition);

    // if we are on the pac, gameover
    if (rp == rg && cp == cg)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

int getGhostNextMove(int ghostIndex)
{
  int m[4][20];
  Cell subNodes[4];
  int i, j, r, c, p, rg, cg, k;

  Cell *head, *current, *tail;
  head = (Cell *)malloc(sizeof(Cell));
  if (head == NULL)
    return -1;

  // where is the pac located
  r = getRow(currentLevel->pacPosition);
  c = getCol(currentLevel->pacPosition);

  // where is the ghost located
  rg = getRow(currentLevel->ghosts[ghostIndex]);
  cg = getCol(currentLevel->ghosts[ghostIndex]);

  // set up our root node
  head->r = r;
  head->c = c;
  head->weight = 0;
  head->next = NULL;

  // give all the cells height weights to begin with
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 20; j++)
    {
      m[i][j] = 1000;
    }
  }

  // make the destination have a weight of 0
  m[r][c] = 0;

  current = head;
  tail = head;

  while (current != NULL)
  {
    // right
    subNodes[0].c = (current->c == 19) ? 0 : current->c + 1;
    subNodes[0].r = current->r;
    // left
    subNodes[1].c = (current->c == 0) ? 19 : current->c - 1;
    subNodes[1].r = current->r;
    // down
    subNodes[2].c = current->c;
    subNodes[2].r = (current->r == 3) ? 0 : current->r + 1;
    // up
    subNodes[3].c = current->c;
    subNodes[3].r = (current->r == 0) ? 3 : current->r - 1;

    // we have found it, return the next move
    if (current->r == rg && current->c == cg)
    {
      int best = 1000;
      int br, bc;
      for (i = 0; i < 4; i++)
      {
        if (m[subNodes[i].r][subNodes[i].c] < best)
        {
          best = m[subNodes[i].r][subNodes[i].c];
          br = subNodes[i].r;
          bc = subNodes[i].c;
        }
      }

      // clear memory

      while ((current = head) != NULL)
      {
        head = head->next;
        free(current);
      }

      return getPosition(br, bc);
    }
    // first we find our neighbours

    subNodes[0].weight = current->weight + 1;
    subNodes[1].weight = current->weight + 1;
    subNodes[2].weight = current->weight + 1;
    subNodes[3].weight = current->weight + 1;

    // add them to the main queue if they are not a wall and their weight is less than existing record in queue
    for (k = 0; k < 4; k++)
    {
      p = getPosition(subNodes[k].r, subNodes[k].c);
      if (isSpaceEmpty(p))
      {
        // if our exising record for this cell is greater than what we juct calculated, update it and add to process queue
        if (m[subNodes[k].r][subNodes[k].c] > subNodes[k].weight)
        {
          m[subNodes[k].r][subNodes[k].c] = subNodes[k].weight;
          // create our new node
          tail->next = (Cell *)malloc(sizeof(Cell));
          tail = tail->next;
          tail->r = subNodes[k].r;
          tail->c = subNodes[k].c;
          tail->weight = subNodes[k].weight;
          tail->next = NULL;
        }
      }
    }

    current = current->next;
  }
}

void moveGhosts()
{
  int moveTo;
  int i, rg, cg, rp, cp, r, c, j, found;
  for (i = 0; i < currentLevel->totalGhosts; i++)
  {
    // where is the ghost
    rg = getRow(currentLevel->ghosts[i]);
    cg = getCol(currentLevel->ghosts[i]);
    // where is the pac
    rp = getRow(currentLevel->pacPosition);
    cp = getCol(currentLevel->pacPosition);

    // if we are on the pac, gameover
    if (rp == rg && cp == cg)
    {
      gameOver();
      return;
    }

    moveTo = getGhostNextMove(i);

    if (moveTo != -1)
    {
      found = 0;
      // now that we have a best move lets make sure that another ghost does not occupy it
      for (j = 0; j < currentLevel->totalGhosts; j++)
      {
        if (j == i)
          continue;

        if (currentLevel->ghosts[j] == moveTo)
        {
          found = 1;
          break;
        }
      }

      // we are not going to collide with another ghost
      if (found == 0)
      {
        r = getRow(moveTo);
        c = getCol(moveTo);
        currentLevel->ghosts[i] = moveTo;
        lcd.setCursor(cg, rg);
        lcd.write(" ");
        lcd.setCursor(c, r);
        lcd.write(byte(ghost1));
      }
    }
  }
  checkGhostHit();
}

void gameOver()
{
  inRunning = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over");
  delay(5000);
}

bool isSpaceEmpty(int space)
{
  int i;
  for (i = 0; i < currentLevel->totalBricks; i++)
  {
    if (currentLevel->bricks[i] == space)
      return false;
  }

  // check if we on a gate and the gate is still enabled
  if (space == currentLevel->gatePosition && currentLevel->gateEnabled == 1)
  {
    return false;
  }

  return true;
}

void buildLevel()
{
  lcd.clear();
  int i, r, c;

  for (i = 0; i < currentLevel->totalBricks; i++)
  {
    r = getRow(currentLevel->bricks[i]);
    c = getCol(currentLevel->bricks[i]);
    lcd.setCursor(c, r);
    lcd.write(0b11111111);
  }

  for (i = 0; i < currentLevel->totalTokens; i++)
  {
    r = getRow(currentLevel->tokens[i]);
    c = getCol(currentLevel->tokens[i]);
    lcd.setCursor(c, r);
    lcd.write(byte(token1));
  }

  for (i = 0; i < currentLevel->totalGhosts; i++)
  {
    r = getRow(currentLevel->ghosts[i]);
    c = getCol(currentLevel->ghosts[i]);
    lcd.setCursor(c, r);
    lcd.write(byte(ghost1));
  }

  r = getRow(currentLevel->goal);
  c = getCol(currentLevel->goal);
  lcd.setCursor(c, r);
  lcd.write("$");

  r = getRow(currentLevel->gatePosition);
  c = getCol(currentLevel->gatePosition);
  lcd.setCursor(c, r);
  lcd.write("#");

  r = getRow(currentLevel->pacPosition);
  c = getCol(currentLevel->pacPosition);
  lcd.setCursor(c, r);
  lcd.write(byte(pacDirection));
}

int getPosition(int r, int c)
{
  return (r * 20) + c;
}

int getRow(byte position)
{
  return (int)floor(position / 20);
}

int getCol(byte position)
{
  return (int)(position % 20);
}
