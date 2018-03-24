#pragma once

#include "types.h"
#include "cdesk.h"
#include "iplayer.h"

class IGame;
class CDesk;
class CMove;
class IAIBase;

/**
* AI driven class, provides some higher abstraction methods permits involvement in the game
*
* @author Martin Holoubek
*/
class CBot : public IPlayer
{
public:
	CBot(IGame* game, const CDesk& board, CSquare player, bool hard = false);
	~CBot();
	void GetMoveAsync();
private:
	bool mHard;
	IAIBase* mAI;
};
