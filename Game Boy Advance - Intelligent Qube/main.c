#include <stdlib.h>
#include <string.h>
#include "mylib.h"
#include "grid.h"
#include "title.h"
#include "yellowcell.h"
#include "graycell.h"
#include "redcell.h"
#include "greencell.h"
#include "strike0.h"
#include "strike1.h"
#include "strike2.h"
#include "strike3.h"
#include "pause.h"
#include "end.h"
#include "numbers.h"


int blockspeed;
int strikes;
static state currentstate;
unsigned char grid_objects[GRID_ROWS * GRID_COLS] = {0};

int main(void)
{
	REG_DISPCNT = MODE3 | BG2_ENABLE;
	Player *p = malloc(sizeof(Player));
	currentstate = TITLE;
	p->row = (GRID_ROWS - 3);
	p->col = (GRID_COLS - 3);
	p->cd = 0;
	p->rd = 0;

	int lvl = 0;
	int pseudotime = 0;

	int bluebombset = 0;
	int greenbombset = 0;
	while (1)
	{
		/**************
		 TITLE SCREEN
		 **************/
		if (currentstate == TITLE) {
			unsigned char temp[GRID_ROWS * GRID_COLS] = {0};
			memcpy(grid_objects, temp, GRID_ROWS * GRID_COLS);
			lvl = 0;
			strikes = 0;
			drawImage3(0, 0, TITLE_WIDTH, TITLE_HEIGHT, title);
			while (!KEY_DOWN_NOW(BUTTON_START)) {}
			while (KEY_DOWN_NOW(BUTTON_START)) {}
			clearScreen();
			currentstate = PLAY;
		/**************
		 GAME SCREEN
		 **************/
		} else if (currentstate == PLAY) {
			pseudotime++;
			if (pseudotime % blockspeed == 0) {
				rotateBlocks(p->row, p->col);
			}
			/**************
		 	 PAUSE SCREEN
		 	**************/
			if (KEY_DOWN_NOW(BUTTON_START)) {
				while (KEY_DOWN_NOW(BUTTON_START)){}
				drawImage3(0, 0, PAUSE_WIDTH, PAUSE_HEIGHT, pause);
				while (!KEY_DOWN_NOW((BUTTON_START | BUTTON_SELECT))){}
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					while (KEY_DOWN_NOW(BUTTON_SELECT)){}
					currentstate = TITLE;
				} else {
					while (KEY_DOWN_NOW(BUTTON_START)){}
					clearScreen();
				}
			}

			//draw grid
			drawImage3(45, 0, GRID_WIDTH, GRID_HEIGHT, grid);
			//draw player
			setcell(p->row, p->col, yellowcell);
			drawStrikesandLevel(lvl);
			if (strikes < 4) {
				int nextlevel = drawBlocks();
				if (nextlevel) {
					lvl++;
					setUpLevel(lvl);
					pseudotime = 0;
					bluebombset = 0;
					greenbombset = 0;
					p->row = (GRID_ROWS - 3);
					p->col = (GRID_COLS - 3);
				}
			} else {
				currentstate = END;
			}

			p->rd = 0;
			p->cd = 0;

			if (KEY_DOWN_NOW(BUTTON_UP) && p->row != 0
				&& (getobjat(p->row - 1, p->col) & BLOCK) == 0) {
				//TODO change this
				while(KEY_DOWN_NOW(BUTTON_UP)){}
				p->rd = -1;
			}
			if (KEY_DOWN_NOW(BUTTON_DOWN) && p->row != GRID_ROWS - 1
				&& (getobjat(p->row + 1, p->col) & BLOCK) == 0) {
				while (KEY_DOWN_NOW(BUTTON_DOWN)){}
				p->rd = 1;
			}
			if (KEY_DOWN_NOW(BUTTON_LEFT) && p->col != 0
				&& (getobjat(p->row, p->col - 1) & BLOCK) == 0){
				while (KEY_DOWN_NOW(BUTTON_LEFT)){}
				p->cd = -1;
			}
			if (KEY_DOWN_NOW(BUTTON_RIGHT) && p->col != GRID_COLS - 1
				&& (getobjat(p->row, p->col + 1) & BLOCK) == 0) {
				while (KEY_DOWN_NOW(BUTTON_RIGHT)){}
				p->cd = 1;
			}

			if (KEY_DOWN_NOW(BUTTON_A)) {
				while (KEY_DOWN_NOW(BUTTON_A)){}
				if (!bluebombset) {
					addobj(p->row, p->col, BLUE_BOMB);
					bluebombset = 1;
				} else {
					greenbombset = detonateblue() | greenbombset;
					bluebombset = 0;
				}
			} if (KEY_DOWN_NOW(BUTTON_B)) {
				while (KEY_DOWN_NOW(BUTTON_B)){}
				if (greenbombset) {
					greenbombset = detonategreen() | greenbombset;
				}
			}


			p->row = p->row + p->rd;
			p->col = p->col + p->cd;
		/**************
		 END / GAME OVER SCREEN
		 **************/
		} else if (currentstate == END) {
			drawImage3(0, 0, END_WIDTH, END_HEIGHT, end);
			printScore(lvl);
			while (!KEY_DOWN_NOW(BUTTON_START)) {}
			while (KEY_DOWN_NOW(BUTTON_START)) {}
			currentstate = TITLE;
		}
		waitForVblank();
	}
}

/* drawimage3
* A function that will draw an arbitrary sized image
* onto the screen (with DMA).
* @param r row to draw the image
* @param c column to draw the image
* @param width width of the image
* @param height height of the image
* @param image Pointer to the first element of the image.
*/
void drawImage3(int r, int c, int width, int height, const u16* image) {
	DMA[3].src = image;
	DMA[3].dst = videoBuffer + (r * 240 + c);
	DMA[3].cnt = width * height | DMA_ON | DMA_DESTINATION_INCREMENT;
}

/* clearScreen
* A function that will clear the screen
* with DMA
*/
void clearScreen() {
	DMA[3].src = 0;
	DMA[3].dst = videoBuffer;
	DMA[3].cnt = 38400 | DMA_ON | DMA_SOURCE_FIXED;
}

/* setCell
* A function that will set a cell (or block) on the grid
* uses DMA
* @param row on the grid
* @param col on the grid
* @cell the image of the colored cell to use
*/
void setcell(int row, int col, const u16* cell) {
	row = (row * CELL_SIZE) + 45;
	col = col * CELL_SIZE + 1;

	for (int i = 1; i < CELL_SIZE - 1; i++) {
		DMA[3].src = cell;
		DMA[3].dst = videoBuffer + OFFSET(row + i + 1, col + 1, 240);
		DMA[3].cnt = (CELL_SIZE - 2) | DMA_ON | DMA_SOURCE_INCREMENT;
	}
}

/** drawStrikesandLevel
 * a method that draws the # of strikes on the screen
 * and the level
 * delegates to draw_image_portion
 * @param lvl of game
 **/
void drawStrikesandLevel(int lvl) {
	const u16* image;
	if (strikes == 0) {
		image = strike0;
	} else if (strikes == 1) {
		image = strike1;
	} else if (strikes == 2) {
		image = strike2;
	} else {
		image = strike3;
	}
	draw_image_portion(15, 160, 0, 0, STRIKE0_WIDTH, STRIKE0_HEIGHT, image, STRIKE0_WIDTH);
	draw_image_portion(8, 15, 0, lvl * 19, 19, NUMBERS_HEIGHT, numbers, NUMBERS_WIDTH);
}

/* drawBorder
* A function that will draw a border on a cell
* by setting pixels on the videoBuffer
* @param row on the grid
* @param col on the grid
* @param color the color to make the border
*/
void drawBorder(int row, int col, u16 color) {
	row = (row * CELL_SIZE) + 45 + 1;
	col = col * CELL_SIZE + 1;
	int w = 0;
	int h = 0;
	while (w < CELL_SIZE) {
		*(videoBuffer + OFFSET(row, col + w, 240)) = color;
		*(videoBuffer + OFFSET(row + CELL_SIZE - 1, col + w, 240)) = color;
		w++;
	}
	while (h < CELL_SIZE) {
		*(videoBuffer + OFFSET(row + h, col, 240)) = color;
		*(videoBuffer + OFFSET(row + h, col + CELL_SIZE - 1, 240)) = color;
		h++;
	}
}

/* drawBlocks
* a function that draws all the active blocks and bombs
* deligates to setcell and drawBorder
* @return 1 or 0 whether it is time to set up a new level or not
* (whether there is nothing on the grid or not)
*/
int drawBlocks() {
	char block;
	u16 color;
	u16 border;
	int row;
	int col;
	int nxtlvl = 1;
	
	for (int i = 0; i < GRID_ROWS * GRID_COLS; i++) {
		if (grid_objects[i] != 0) {
			border = 0;
			color = 0;
			block = grid_objects[i];
			if (block != 0) {
				row = getrow(i);
				col = getcol(i);
				if (block & GRAY_BLOCK) {
					setcell(row, col, graycell);
					nxtlvl = 0;
				} else if (block & RED_BLOCK) {
					setcell(row, col, redcell);
					nxtlvl = 0;
				} else if (block & GREEN_BLOCK) {
					setcell(row, col, greencell);
					nxtlvl = 0;
				}
				if (block & GREEN_BOMB & BLUE_BOMB) {
					drawBorder(row, col, CYAN);
				}else if (block & BLUE_BOMB) {
					drawBorder(row, col, BLUE);
				}else if (block & GREEN_BOMB) {
					drawBorder(row, col, GREEN);
				}
			}
		}
	}
	return nxtlvl;
}

/** addobj
 * adds an object on the grid
 * puts it in the array grid_objects
 * @param row on the grid to add to
 * @param col on the grid to add to
 * @param obj to add
 */
void addobj(int row, int col, char obj) {
	grid_objects[row * GRID_COLS + col] = grid_objects[row * GRID_COLS + col] | obj;
}

/** getobjat
 * gets the object at specified location
 * @param row desired
 * @param col desired
 * @return the object at this location
 */
char getobjat(int row, int col) {
	return grid_objects[row * GRID_COLS + col];
}

/** getrow
 * gets the row of some index in grid_objects
 * @param i the index of grid_objects
 * @return the row on the grid it would correspond to
 */
int getrow(int i) {
	return (int) (i / GRID_COLS);
}

/** getcol
 * gets the column of some index in grid_objects
 * @param i the index of grid_objects
 * @return the column on the grid it would correspond to
 */
int getcol (int i) {
	return i - (GRID_COLS * getrow(i));
}

/** detonateblue
 * finds where the bomb is and removes the block in that location
 * @return 1 or 0 whether a green bomb was set
 */
int detonateblue() {
	for (int i = 0; i < GRID_ROWS * GRID_COLS; i++) {
		if (grid_objects[i] & BLUE_BOMB) {
			//get rid of all the other objs here except for a green bomb if present
			if (grid_objects[i] & RED_BLOCK) {
				grid_objects[i] = grid_objects[i] & GREEN_BOMB;
				//do some punishment
				strikes++;
			} else if (grid_objects[i] & GRAY_BLOCK) {
				grid_objects[i] = grid_objects[i] & GREEN_BOMB;
				//do some reward
			} else if (grid_objects[i] & GREEN_BLOCK) {
				//add a green bomb
				grid_objects[i] = (grid_objects[i] & 0) | GREEN_BOMB;
				return 1;
			} else {
				//empty cell
				grid_objects[i] = grid_objects[i] & GREEN_BOMB;
			}
			return 0;
		}
	}
	return 0;
}

/** detonategreen
 * finds where the bomb(s) is/are and removes the block in that/those location(s)
 * @return 1 or 0 whether a green bomb was set
 */
int detonategreen() {
	int greenset = 0;
	int indices[9];
	unsigned char temp[GRID_ROWS * GRID_COLS];
	memcpy(temp, grid_objects, GRID_ROWS * GRID_COLS);

	for (int i = 0; i < GRID_ROWS * GRID_COLS; i++) {
		if (temp[i] & GREEN_BOMB) {
			indices[0] = i - GRID_COLS - 1;
			indices[1] = i - GRID_COLS;
			indices[2] = (i - GRID_COLS) + 1;
			indices[3] = i - 1;
			indices[4] = i;
			indices[5] = i + 1;
			indices[6] = (i + GRID_COLS) - 1;
			indices[7] = i + GRID_COLS;
			indices[8] = i + GRID_COLS + 1;

			for (int j = 0; j < 9; j++) {
				//if ((indices[j] + 1 % GRID_COLS == 0 && j % 3 == 0)
					//x|| (indices[j] % GRID_COLS == 0 && (j == 2 || j == 5 || j == 8))) {
					//gross edge cases
				if (indices[j] > -1 && indices[j] < (GRID_COLS * GRID_ROWS)) {
					if (temp[indices[j]] & RED_BLOCK) {
						grid_objects[indices[j]] = grid_objects[indices[j]] & BLUE_BOMB;
						//do some punishment
						strikes++;
					} else if (temp[indices[j]] & GRAY_BLOCK) {
						grid_objects[indices[j]] = grid_objects[indices[j]] & BLUE_BOMB;
					} else if (temp[indices[j]] & GREEN_BLOCK) {
						//add a green bomb
						grid_objects[indices[j]] = (grid_objects[indices[j]] & 0) | GREEN_BOMB;
						greenset = 1;
					} else {
						//nothing here
						grid_objects[indices[j]] = grid_objects[indices[j]] & BLUE_BOMB;
					}
				}
			}
		}
	}
	return greenset;
}

/** rotateBlocks
 * rotates the blocks by moving them up in the array grid_objects
 * adds strikes if a non-red block falls off grid
 * speeds up blocks if the player gets "run over"
 * @param pRow the row of the player's position
 * @param pCol the column of the player's position
 */
void rotateBlocks(int pRow, int pCol) {
	for (int i = GRID_ROWS * GRID_COLS -  1; i > 0; i--) {
		if ((i + 1) % GRID_COLS == 0 && 
			(grid_objects[i] & (GREEN_BLOCK | GRAY_BLOCK))) {
			//do some punishment
			//a non red block fell off the screen
			strikes++;
			
		}

		if (i % GRID_COLS != 0) {
			grid_objects[i] = (grid_objects[i - 1] & BLOCK) | (grid_objects[i] & BOMB);
			grid_objects[i - 1] = (grid_objects[i - 1] & (GREEN_BOMB | BLUE_BOMB));
			if ((grid_objects[i] & BLOCK) && getrow(i) == pRow && getcol(i) == pCol) {
				//player has been run over
				blockspeed = 10;
			}
		}
	}
}

/** printScore
 * calculates iq and puts it on the screen at the end of the game
 * @param lvl for purpose of calculating score
 */
void printScore(int lvl) {
	int iq = ((lvl) * 20);
	iq = iq + ((4 - strikes) * 10);
	int firstDigit = (int) (iq / 100);
	int row = 68;
	int col = 120;
	int printednumbers = 0;
	if (firstDigit != 0) {
		draw_image_portion(row, col, 0, firstDigit * 19, 19, NUMBERS_HEIGHT, numbers, NUMBERS_WIDTH);
		printednumbers++;
	}
	int secondDigit = (int)(iq - (firstDigit * 100))/10;
	if (!(firstDigit == 0 && secondDigit == 0)) {
		draw_image_portion(row, col + printednumbers * 19, 0, secondDigit * 19, 19, NUMBERS_HEIGHT, numbers, NUMBERS_WIDTH);
		printednumbers++;
	}
	int thirdDigit = (int) (iq - (firstDigit * 100) - (secondDigit * 10));
	draw_image_portion(row, col + printednumbers * 19, 0, thirdDigit * 19, 19, NUMBERS_HEIGHT, numbers, NUMBERS_WIDTH);
}

/**
 * This function draws a portion of an arbitrary sized image on the screen at a certain point using DMA.
 * (taken from my TL3 solution)
 * @ destination_row: the row on the screen that the portion of the image should be drawn at
 * @ destination_col: the column on the screen that the portion of the image should be drawn at
 * @ source_row: the row of the source image that begins the portion of the smaller image to be drawn
 * @ source_col: the column of the source image that begins the portion of the smaller image to be drawn
 * @ source_width: the width of the portion of the image to be drawn 
 * @ source_height: the height of the portion of the image to be drawn
 * @ image: the full source image
 * @ image_width: the width of the full source image
 */
void draw_image_portion(int destination_row, int destination_col, int source_row, int source_col, int source_width, int source_height, const u16* image, int image_width)
{
    for (int i = 0; i < source_height; i++) {
        DMA[3].src = image + OFFSET(source_row + i, source_col, image_width);
        DMA[3].dst = videoBuffer + OFFSET(destination_row + i, destination_col, 240);
        DMA[3].cnt = source_width | DMA_ON | DMA_SOURCE_INCREMENT;
    }
}

/** setUpLevel
 * clears the grid_objects array and fills it with blocks
 * sets a different blockspeed
 * @param lvl the level to set up
 */
void setUpLevel(int lvl) {
	//clear grid_objects
	unsigned char temp[GRID_ROWS * GRID_COLS] = {0};
	memcpy(grid_objects, temp, GRID_ROWS * GRID_COLS);
	if (lvl == 1) {
		blockspeed = 150;
		addobj(0, 0, RED_BLOCK);
		addobj(0, 1, GRAY_BLOCK);
		addobj(1, 0, GRAY_BLOCK);
		addobj(1, 1, GRAY_BLOCK);
		addobj(2, 0, GREEN_BLOCK);
		addobj(2, 1, GRAY_BLOCK);
		addobj(3, 0, GRAY_BLOCK);
		addobj(3, 1, GRAY_BLOCK);
	} else if (lvl == 2) {
		blockspeed = 150;
		addobj(0, 0, GRAY_BLOCK);
		addobj(0, 1, GRAY_BLOCK);
		addobj(0, 2, GREEN_BLOCK);
		addobj(1, 0, GRAY_BLOCK);
		addobj(1, 1, GRAY_BLOCK);
		addobj(1, 2, GRAY_BLOCK);
		addobj(2, 0, GRAY_BLOCK);
		addobj(2, 1, GRAY_BLOCK);
		addobj(2, 2, RED_BLOCK);
		addobj(3, 0, GREEN_BLOCK);
		addobj(3, 1, GRAY_BLOCK);
		addobj(3, 2, GRAY_BLOCK);
	} else if (lvl == 3) {
		blockspeed = 100;
		addobj(0, 0, GRAY_BLOCK);
		addobj(0, 1, GRAY_BLOCK);
		addobj(0, 2, RED_BLOCK);
		addobj(1, 0, GRAY_BLOCK);
		addobj(1, 1, GRAY_BLOCK);
		addobj(1, 2, GREEN_BLOCK);
		addobj(2, 0, GRAY_BLOCK);
		addobj(2, 1, GRAY_BLOCK);
		addobj(2, 2, RED_BLOCK);
		addobj(3, 0, GRAY_BLOCK);
		addobj(3, 1, GREEN_BLOCK);
		addobj(3, 2, RED_BLOCK);
	}else if (lvl == 4) {
		blockspeed = 150;
		addobj(0, 0, GRAY_BLOCK);
		addobj(0, 1, GREEN_BLOCK);
		addobj(0, 2, GRAY_BLOCK);
		addobj(0, 3, GRAY_BLOCK);
		addobj(0, 4, GRAY_BLOCK);
		addobj(1, 0, GRAY_BLOCK);
		addobj(1, 1, GREEN_BLOCK);
		addobj(1, 2, RED_BLOCK);
		addobj(1, 3, RED_BLOCK);
		addobj(1, 4, GRAY_BLOCK);
		addobj(2, 0, GRAY_BLOCK);
		addobj(2, 1, GRAY_BLOCK);
		addobj(2, 2, GRAY_BLOCK);
		addobj(2, 3, GRAY_BLOCK);
		addobj(2, 4, RED_BLOCK);
		addobj(3, 0, RED_BLOCK);
		addobj(3, 1, GRAY_BLOCK);
		addobj(3, 2, GRAY_BLOCK);
		addobj(3, 3, GREEN_BLOCK);
		addobj(3, 4, RED_BLOCK);
	} else if (lvl == 5) {
		blockspeed = 150;
		addobj(0, 0, RED_BLOCK);
		addobj(0, 1, GRAY_BLOCK);
		addobj(0, 2, GRAY_BLOCK);
		addobj(0, 3, GRAY_BLOCK);
		addobj(0, 4, RED_BLOCK);
		addobj(1, 0, GRAY_BLOCK);
		addobj(1, 1, GRAY_BLOCK);
		addobj(1, 2, GRAY_BLOCK);
		addobj(1, 3, GRAY_BLOCK);
		addobj(1, 4, GREEN_BLOCK);
		addobj(2, 0, RED_BLOCK);
		addobj(2, 1, GRAY_BLOCK);
		addobj(2, 2, GRAY_BLOCK);
		addobj(2, 3, GREEN_BLOCK);
		addobj(2, 4, RED_BLOCK);
		addobj(3, 0, RED_BLOCK);
		addobj(3, 1, GRAY_BLOCK);
		addobj(3, 2, GRAY_BLOCK);
		addobj(3, 3, GRAY_BLOCK);
		addobj(3, 4, RED_BLOCK);
	} else if (lvl == 6) {
		blockspeed = 100;
		addobj(0, 0, GRAY_BLOCK);
		addobj(0, 1, GRAY_BLOCK);
		addobj(0, 2, RED_BLOCK);
		addobj(0, 3, GREEN_BLOCK);
		addobj(0, 4, RED_BLOCK);
		addobj(1, 0, GRAY_BLOCK);
		addobj(1, 1, GRAY_BLOCK);
		addobj(1, 2, RED_BLOCK);
		addobj(1, 3, GRAY_BLOCK);
		addobj(1, 4, RED_BLOCK);
		addobj(2, 0, RED_BLOCK);
		addobj(2, 1, GRAY_BLOCK);
		addobj(2, 2, GRAY_BLOCK);
		addobj(2, 3, GRAY_BLOCK);
		addobj(2, 4, GRAY_BLOCK);
		addobj(3, 0, RED_BLOCK);
		addobj(3, 1, RED_BLOCK);
		addobj(3, 2, GRAY_BLOCK);
		addobj(3, 3, GRAY_BLOCK);
		addobj(3, 4, GREEN_BLOCK);
	} else {
		currentstate = END;
	}
}