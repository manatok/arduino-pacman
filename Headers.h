#define byte uint8_t

int pacRight = 2;
byte pac_right[8] = {
	0b00111,
	0b01110,
	0b11100,
	0b11000,
	0b11100,
	0b01110,
	0b00111
};

int pacLeft = 3;
byte pac_left[8] = {
	0b11100,
	0b01110,
	0b00111,
	0b00011,
	0b00111,
	0b01110,
	0b11100
};

int pacUp = 0;
byte pac_up[8] = {
	0b00000,
	0b10001,
	0b11011,
	0b11111,
	0b11111,
	0b01110,
	0b00000
};

int pacDown = 1;
byte pac_down[8] = {
	0b00000,
	0b01110,
	0b11111,
	0b11111,
	0b11011,
	0b10001,
	0b00000
};

int pacClosed = 4;
byte pac_closed[8] = {
	0b00000,
	0b01110,
	0b11111,
	0b11111,
	0b11111,
	0b01110,
	0b00000
};

int ghost1 = 5;
byte ghost_1[8] = {
	0b00100,
	0b01110,
	0b10101,
	0b11111,
	0b11111,
	0b10101,
	0b00000
};

int ghost2 = 6;
byte ghost_2[8] = {
	0b00100,
	0b01110,
	0b10101,
	0b11111,
	0b11111,
	0b01010,
	0b00000
};

int token1 = 7;
byte token_1[8] = {
	0b00100,
	0b01010,
	0b00100,
	0b01010,
	0b00100,
	0b01010,
	0b00100
};

int token2 = 8;
byte token_2[8] = {
	0b00100,
	0b01010,
	0b01010,
	0b01010,
	0b01010,
	0b01010,
	0b00100
};

typedef struct cell_el {
  byte r;
  byte c;
  int weight;
  struct cell_el *next;
} Cell;




