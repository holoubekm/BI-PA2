#include "iplayer.h"
#include "igame.h"
#include "cdesk.h"

/**
* Constructor
* @param game pointer to main game class to make callbacks possible
* @param board reference to CDesk instance, players gameboard
* @param player player's color
*/
IPlayer::IPlayer(IGame* game, const CDesk& board, CSquare player) 
: mPlayer(player), mGame(game), mBoard(board) {}

/**
* Method to check whether the move is playable by current player
* @param move value to test
* @return specifies the possibility
*/
bool IPlayer::AllowedMove(const CMove& move)
{
	if(mBoard.mDesk[move.mRow][move.mCol] != mEmpty) return false;
	CDesk buf(mBoard);
	return buf.PerformTurn(move, mPlayer) > 0;
}

/**
* Checks if the players has at leat one possible turn
* @return specifies the ability to play
*/
bool IPlayer::CanPlay()
{
	set<CMove, CMove>::const_iterator beg;
	set<CMove, CMove>::const_iterator end = mBoard.mPosMoves.end();

	for(beg = mBoard.mPosMoves.begin(); beg != end; ++beg)
	{
		CDesk buf(mBoard);
		int price = buf.PerformTurn(*beg, mPlayer);
		if(price > 0)
		{
			return true;
		}
	}
	return false;
}

/**
* Method return opponent's color
* @return opponents color
*/
CSquare IPlayer::GetOpponent()
{
	return mPlayer == mWhite ? mBlack : mWhite;
}

/**
* Perform opponents move to fix the possible moves list
*/
void IPlayer::SetOppMove(const CMove& move)
{
	mBoard.PerformTurn(move, GetOpponent());
}

/**
* Dummy method, should be overridden
* @return means nothing
*/
bool IPlayer::NextFrame() 
{
	return true;
}

/**
* Dummy method, should be overridden
*/
void IPlayer::GetMoveAsync() 
{

}
