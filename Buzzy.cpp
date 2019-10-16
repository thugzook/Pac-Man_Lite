#include "Buzzy.h"
#include "Ghosts.h"
#include "BuzzyGraphics.h"
#include "uLCD_4DGL.h"
#include "Speaker.h"

extern char gDynaMaze[MAZE_NUM_ROW][MAZE_NUM_COL];
extern Ghosts gGhosts[NUM_GHOSTS];
extern Buzzy gBuzzy;
extern uLCD_4DGL guLCD;
extern Speaker gSpeakerOut;
extern int honey_count;

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// Constructor
Buzzy::Buzzy(enDIRECTIONS inDir, unsigned int inRow, unsigned int inCol):
        Sprite(inDir, inRow, inCol)
{
    m_CurrentDirection = m_DesiredDirection = Sprite::LEFT_DIR;
    m_nActiveImage=0;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Buzzy::Move()
{
    switch (m_DesiredDirection) {
        case UP_DIR:
            if (IsMoveAllowed(m_RowPos - 1, m_ColPos))
            {
                m_CurrentDirection = UP_DIR;
                DrawInNewLocation(m_RowPos - 1, m_ColPos);
                return;
            }
            gSpeakerOut.SwitchSound(Speaker::CHOMP);
            break;
        case DOWN_DIR:
            if (IsMoveAllowed(m_RowPos + 1, m_ColPos))
            {
                m_CurrentDirection = DOWN_DIR;
                DrawInNewLocation(m_RowPos + 1, m_ColPos);
                return;
            }
            gSpeakerOut.SwitchSound(Speaker::CHOMP);
            break;
        case RIGHT_DIR:
            if (IsMoveAllowed(m_RowPos, m_ColPos + 1))
            {
                m_CurrentDirection = RIGHT_DIR;
                if (m_RowPos == 19 && m_ColPos == 40) {
                    DrawInNewLocation(m_RowPos, 2);
                    gDynaMaze[19][41] = 0;
                    return;
                }
                DrawInNewLocation(m_RowPos, m_ColPos + 1);
                return;
            }
            gSpeakerOut.SwitchSound(Speaker::CHOMP);
            break;
        case LEFT_DIR:
            if (IsMoveAllowed(m_RowPos, m_ColPos - 1))
            {
                m_CurrentDirection = LEFT_DIR;
                if (m_RowPos == 19 && m_ColPos == 2) {
                    DrawInNewLocation(m_RowPos, 40);
                    return;
                }
                DrawInNewLocation(m_RowPos, m_ColPos - 1);
                return;
            }
            gSpeakerOut.SwitchSound(Speaker::CHOMP);
            break;
    }

    if (m_DesiredDirection == NO_DIR)
    {
        // Nothing to do
        // Switch over to the siren sound
        gSpeakerOut.SwitchSound(Speaker::SIREN);
        return;
    }
    // Try to move buzzy in the desired direction
    switch (m_CurrentDirection) {
        case UP_DIR:
            if (IsMoveAllowed(m_RowPos - 1, m_ColPos))
            {
                DrawInNewLocation(m_RowPos - 1, m_ColPos);
                return;
            }
            gSpeakerOut.SwitchSound(Speaker::CHOMP);
            break;
        case DOWN_DIR:
            if (IsMoveAllowed(m_RowPos + 1, m_ColPos))
            {
                DrawInNewLocation(m_RowPos + 1, m_ColPos);
                return;
            }
            gSpeakerOut.SwitchSound(Speaker::CHOMP);
            break;
        case RIGHT_DIR:
            if (IsMoveAllowed(m_RowPos, m_ColPos + 1))
            {
                if (m_RowPos == 19 && m_ColPos == 40) {
                    DrawInNewLocation(m_RowPos, 2);
                    gDynaMaze[19][41] = 0;
                    return;
                }
                DrawInNewLocation(m_RowPos, m_ColPos + 1);
                return;
            }
            gSpeakerOut.SwitchSound(Speaker::CHOMP);
            break;
        case LEFT_DIR:
            if (IsMoveAllowed(m_RowPos, m_ColPos - 1))
            {
                if (m_RowPos == 19 && m_ColPos == 2) {
                    DrawInNewLocation(m_RowPos, 40);
                    return;
                }
                DrawInNewLocation(m_RowPos, m_ColPos - 1);
                return;
            }
            gSpeakerOut.SwitchSound(Speaker::CHOMP);
            break;
    }
    m_DesiredDirection = NO_DIR;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Buzzy::DrawInNewLocation(const int &nRow,const int &nCol)
{
    int x1, y1;
    // Test if we hit a honeydrop or fruit or ghost
    switch (gDynaMaze[nRow][nCol])
    {
        case HONEYDROP_SQUARE:
            honey_count--;
            gDynaMaze[nRow][nCol] = 0;
            break;
        case GHOST_ICON:
            // see if pacman is in power up state
            // kill if so, etc.
            break;
        case PWRUP_SQUARE:
            // make pacman pwr'd up
            gDynaMaze[nRow][nCol] = 0;
            break;
    }

    // Clean up the part that will be left
    int ii = 3*m_RowPos;
    int jj = 3*m_ColPos;
    guLCD.filled_rectangle(ii-3, jj-3, ii+5, jj+5, BLACK);

    gBuzzy.SetLocation(nRow, nCol);
    x1 = (3*m_RowPos)-3;
    y1 = (3*m_ColPos)-3;
    guLCD.BLIT(x1, y1, 9, 9, &BuzzyIcon[GetImageIndex()][0][0]);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Buzzy::DidGhostGetBuzzy()
{

    return false;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Buzzy::IsMoveAllowed(const int &nNewRow, const int &nNewCol)
{
    // Check available moves in a + pattern (if o is pacman)
    /*   x x x
     * x x x x x
     * x x o x x
     * x x x x x
     *   x x x
     */

    // up direction
    if (nNewRow < m_RowPos)
    {
        if (gDynaMaze[m_RowPos - 1][m_ColPos - 1] == BLUE_SQUARE ||
                gDynaMaze[m_RowPos - 2][m_ColPos - 1] == BLUE_SQUARE ||
                gDynaMaze[m_RowPos - 1][m_ColPos] == BLUE_SQUARE ||
                gDynaMaze[m_RowPos - 2][m_ColPos] == BLUE_SQUARE ||
                gDynaMaze[m_RowPos - 1][m_ColPos + 1] == BLUE_SQUARE ||
                gDynaMaze[m_RowPos - 2][m_ColPos + 1] == BLUE_SQUARE)
        {
            return false;
        }
    }
    // down direction
    else if (nNewRow > m_RowPos)
    {
        if (gDynaMaze[m_RowPos + 1][m_ColPos - 1] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos + 2][m_ColPos - 1] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos + 1][m_ColPos] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos + 2][m_ColPos] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos + 1][m_ColPos + 1] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos + 2][m_ColPos + 1] == BLUE_SQUARE)
        {
            return false;
        }
    }
    // left direction
    else if (nNewCol < m_ColPos)
    {
        if (gDynaMaze[m_RowPos - 1][m_ColPos - 1] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos - 1][m_ColPos - 2] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos][m_ColPos - 1] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos][m_ColPos - 2] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos + 1][m_ColPos - 1] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos + 1][m_ColPos - 2] == BLUE_SQUARE)
        {
            return false;
        }
    }
    // right direction
    else
    {
        if (gDynaMaze[m_RowPos - 1][m_ColPos + 1] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos - 1][m_ColPos + 2] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos][m_ColPos + 1] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos][m_ColPos + 2] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos + 1][m_ColPos + 1] == BLUE_SQUARE ||
            gDynaMaze[m_RowPos + 1][m_ColPos + 2] == BLUE_SQUARE)
        {
            return false;
        }
    }
    return true;
}
