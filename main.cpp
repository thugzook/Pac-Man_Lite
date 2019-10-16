#include "mbed.h"
#include <stdio.h>
#include "Speaker.h"
#include "PinDetect.h"
#include "BuzzyGraphics.h"
#include "uLCD_4DGL.h"

#include "Buzzy.h"
#include "Ghosts.h"
////////////////////////////////////////
// Setup instance of LCD display
uLCD_4DGL guLCD(p28, p27, p29); // serial tx, serial rx, reset pin;
////////////////////////////////////////
// Setup instances of push button pins
PinDetect gPB_left(p16);
PinDetect gPB_right(p17);
PinDetect gPB_up(p19);
PinDetect gPB_down(p18);
// Create Buzzy and Ghosts
Buzzy gBuzzy(Sprite::LEFT_DIR, 25,20);
Ghosts gGhosts[NUM_GHOSTS];
// Variable indicates if game is paused or running
int gGameState = GAME_PAUSED;
// Declare and initialize the speaker
Speaker gSpeakerOut(p21);
extern int ghost_count;
int honey_count = 0;

////////////////////////////////////////////////////////
// This is the maze that changes as the game is played
char gDynaMaze[MAZE_NUM_ROW][MAZE_NUM_COL];

//////////////////////////
// Prototype functions
void DrawMaze();

//////////////////////////////////////////////////////////////////////
// Interrupt routine
// used to output next analog sample whenever a timer interrupt occurs
void Sample_timer_interrupt(void)
{
    // Call speaker function to play next value
    if (gSpeakerOut.getBuzzy_Sound() == Speaker::BEGIN || gSpeakerOut.getBuzzy_Sound() == Speaker::NO_SOUND) {}
    else
    {
        gSpeakerOut.PlayNextValue();
    }
    wait(0.00001);
}
//---------------------------------------------------------------------------------------------------
// Callback routine is interrupt activated by a debounced pb_left hit
void pb_left_hit_callback (void)
{
    // Update game state and tell Buzzy to go left
    gBuzzy.SetDesiredDirectionToMove(Sprite::LEFT_DIR);
    gGameState = GAME_RUNNING;
}
//---------------------------------------------------------------------------------------------------
// Callback routine is interrupt activated by a debounced pb_right hit
void pb_right_hit_callback (void)
{
    // Update game state and tell Buzzy to go up
    gBuzzy.SetDesiredDirectionToMove(Sprite::RIGHT_DIR);
    gGameState = GAME_RUNNING;
}
//---------------------------------------------------------------------------------------------------
// Callback routine is interrupt activated by a debounced pb_up hit
void pb_up_hit_callback (void)
{
    // Update game state and tell Buzzy to go up
    gBuzzy.SetDesiredDirectionToMove(Sprite::UP_DIR);
    gGameState = GAME_RUNNING;
}
//---------------------------------------------------------------------------------------------------
// Callback routine is interrupt activated by a debounced pb_down hit
void pb_down_hit_callback (void)
{
    // Update game state and tell Buzzy to go down
    gBuzzy.SetDesiredDirectionToMove(Sprite::DOWN_DIR);
    gGameState = GAME_RUNNING;
}
//---------------------------------------------------------------------------------------------------
int main()
{

    // Setup push buttons
    gPB_left.mode(PullUp);
    gPB_right.mode(PullUp);
    gPB_up.mode(PullUp);
    gPB_down.mode(PullUp);
    // Delay for initial pullup to take effect
    wait(.01);
    // Setup Interrupt callback functions for a pb hit
    gPB_left.attach_deasserted(&pb_left_hit_callback);
    gPB_right.attach_deasserted(&pb_right_hit_callback);
    gPB_up.attach_deasserted(&pb_up_hit_callback);
    gPB_down.attach_deasserted(&pb_down_hit_callback);
    // Setup speaker
    gSpeakerOut.period(1.0/200000.0);
    // set up a timer to be used for sample rate interrupts
    Ticker Sample_Period;
    Sample_Period.attach(&Sample_timer_interrupt, 1.0/(20000.0));

    //Setup LCD display
    guLCD.display_control(LANDSCAPE);
    guLCD.background_color(BLACK);
    guLCD.cls();
    guLCD.baudrate(BAUD_3000000); //jack up baud rate to max for fast display
    wait(1.0);

    // Start sampling pb inputs using interrupts
    gPB_left.setSampleFrequency();
    gPB_right.setSampleFrequency();
    gPB_up.setSampleFrequency();
    gPB_down.setSampleFrequency();
    //////////////////////////////////////
    // Everything should be ready to start playing the game.
    while(1)
    {
        guLCD.cls();
        // Ask the user if they would like to play a game.
        bool isPrompted = false;

        // Wait for a button to be pressed
        while (gGameState == GAME_PAUSED)
        {
            if (!isPrompted)
            {
                guLCD.printf("Press any button to play.");
                isPrompted = true;
            }
            wait(0.0001);
        }
        guLCD.printf("HAIHA");
        gGameState = GAME_RUNNING;
        guLCD.cls();
        // Initialize needed parts

        // Copy CnstMaze to DynaMaze
        for (int i = 0 ; i < MAZE_NUM_ROW ; i++)
        {
            for (int j = 0 ; j < MAZE_NUM_COL ; j++)
            {
                gDynaMaze[i][j] = gCnstMaze[i][j];
            }
        }

        // Reset the Ghost states
        ghost_count = 0;

        // Count honeydews
        for (int ii = 0; ii < MAZE_NUM_ROW; ++ii) {
            for (int jj = 0; jj < MAZE_NUM_COL; ++jj) {
                if (gDynaMaze[ii][jj] == HONEYDROP_SQUARE) {
                    ++honey_count;
                }
            }
        }

        // Start up new game
        // Play introduction sounds while drawing the Maze
        gSpeakerOut.SwitchSound(Speaker::BEGIN);
        int val = gSpeakerOut.GetNextValue();
        for (int i = 0; i < NUM_BEGIN_ELEMENTS; ++i){
            wait(0.00008);
            gSpeakerOut.PlayNextValue();
        }
        wait(2);
        guLCD.display_control(PORTRAIT);
        DrawMaze();
        gSpeakerOut.SwitchSound(Speaker::NO_SOUND);

        // Start Game loop
        while (gGameState == GAME_RUNNING)
        {
            // Move Buzzy and any active ghosts
            gSpeakerOut.SwitchSound(Speaker::CHOMP);
            gBuzzy.Move();
            // Check to see if Buzzy has eaten all the honey drops
            if (honey_count == 0) {
                gGameState = GAME_PAUSED;
            }
            // Break out of loop if all honey drops are consumed
            wait(0.001);
        }
        gBuzzy.SetLocation(25, 20);
        gSpeakerOut.SwitchSound(Speaker::NO_SOUND);
        gGameState = GAME_PAUSED;
    }

} //end mains