#pragma once

#include "types.h"
#include "cdesk.h"
#include "iplayer.h"

class IGame;
class CWindow;

/**
* Class to interface human player and the game, for an obvious reason 
* it needs to reference the window instance
*
* @author Martin Holoubek
*/
class CHuman : public IPlayer
{
public:
	CHuman(IGame* game, CWindow* window, const CDesk& board, CSquare player);
	void GetMoveAsync();
private:
	CWindow* mWindow;
};
 