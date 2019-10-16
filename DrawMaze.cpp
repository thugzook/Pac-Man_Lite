#include "uLCD_4DGL.h"
#include "BuzzyGraphics.h"
#include "Buzzy.h"

extern uLCD_4DGL guLCD;
extern char gDynaMaze[MAZE_NUM_ROW][MAZE_NUM_COL];
extern Buzzy gBuzzy;
int ghost_count = 0;
/////////////////////////////////////////////////////////////////////
// The maze is a scaled down version of the LCD display
// Each element in the maze is really a 3x3 segment of the LCD
// This must be taken into account when drawing the maze on the LCD


// Draw a wall tile when 
void DrawMazeWall(int const &x1, int const &y1)
{
    int ii = 3*x1+1;
    int jj = 3*y1+1;
    guLCD.filled_rectangle(ii-1, jj-1, ii+1, jj+1, _BLUE);
}
/*
//////////////////////////////////////////////////////////////////////
Use the following #defines to determine which icon to draw in the LCD
#define BLUE_SQUARE 1
#define HONEYDROP_SQUARE 2
#define PWRUP_SQUARE 3
#define GHOST_ICON 4
#define BUZZY_ICON 5
#define TRAP_LINE 6

When drawing the ghosts draw one of each color
*/
void DrawMaze()
{
    // Draw that maze
    for (int i = 0 ; i < MAZE_NUM_ROW ; i++)
    {
        for (int j = 0 ; j < MAZE_NUM_COL ; j++)
        {
            // for 9 by 9 objects, must populate 9x9 space
            switch(gDynaMaze[i][j]) {
                case BLUE_SQUARE:
                    DrawMazeWall(i, j);
                    break;
                case HONEYDROP_SQUARE:
                    // loc + 1 - 4 so that the positioning of the BLIT is in the center of the honey drop
                    guLCD.BLIT((i*3), (j*3), 3, 3, &HoneyDropIcon[0][0]);
                    break;
                case PWRUP_SQUARE:
                    // Found on official mbed documentation
                    guLCD.BLIT(i*3 + 1 - 4, j*3 + 1 - 4, 9, 9, &PowerUpIcon[0][0]);
                    break;
                case GHOST_ICON:
                    switch (ghost_count) {
                        // violet ghost
                        case 0:
                            guLCD.BLIT(i * 3 - 3, j * 3 - 3, 9, 9, &VioletGhost[0][0][0]);
                            ++ghost_count;
                            break;
                            // blue ghost
                        case 1:
                            guLCD.BLIT(i * 3 - 3, j * 3 - 3, 9, 9, &VioletGhost[0][0][0]);
                            ++ghost_count;
                            break;
                            // pink ghost
                        case 2:
                            guLCD.BLIT(i * 3 - 3, j * 3 - 3, 9, 9, &PinkGhost[0][0][0]);
                            ++ghost_count;
                            break;
                            // green ghost
                        case 3:
                            guLCD.BLIT(i * 3 - 3, j * 3 - 3, 9, 9, &GreenGhost[0][0][0]);
                            ++ghost_count;
                            break;
                    }
                    break;
                case BUZZY_ICON:
                    guLCD.BLIT(i*3 - 3, j*3 - 3, 9, 9, &BuzzyIcon[0][0][0]);
                    break;
                case TRAP_LINE:
                    guLCD.BLIT(i*3 + 1 - 4, j*3 + 1 - 4, 9, 9, &TrapLine[0][0]);
                    break;
            }
        }
    }
}
