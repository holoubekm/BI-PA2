#include <cstdlib>

#include "igame.h"
#include "iplayer.h"
#include "caiminimax.h"
#include "caisimple.h"

#include "cbot.h"

/**
* Constructor
* @param game pointer to main game class to make callbacks possible
* @param board reference to CDesk instance, players gameboard
* @param player player's color
* @param hard defines difficulty and the abilities of the AI
*/
CBot::CBot(IGame* game, const CDesk& board, CSquare player, bool hard) : IPlayer(game, board, player), mHard(hard)
{
	mHard = false;
	if(hard)
	{
		mAI = new CAIMinimax();
	}
	else
	{
		mAI = new CAISimple();
	}
}

/**
* Destructor, deletes objects in a safe way
*/
CBot::~CBot()
{
	DeleteObject(mAI);
}

/**
* Retrieves next move from the AI interface, performs IGame callback and even the chosen turn
*/
void CBot::GetMoveAsync()
{
	CMove move = mAI->Solve(mBoard, mPlayer);
	mBoard.PerformTurn(move, mPlayer);
	mGame->CallbackGotMove(this, move);
}