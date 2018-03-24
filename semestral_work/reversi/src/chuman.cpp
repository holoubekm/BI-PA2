#include "chuman.h"
#include "igame.h"
#include "cwindow.h"

/**
* Constructor
* @param game pointer to main game class to make callbacks possible
* @param window pointer to the CWindow instance, allowing actual user input
* @param board reference to CDesk instance, players gameboard
* @param player player's color
*/
CHuman::CHuman(IGame* game, CWindow* window, const CDesk& board, CSquare player) : IPlayer(game, board, player), mWindow(window)
{
	
}

/**
* Method to load input from the current window, calls game's callback method and performs turn on the inner game board
*/
void CHuman::GetMoveAsync()
{
	CMove move;
	do
	{
		move = mWindow->GetMove(mPlayer);
	}
	while(!AllowedMove(move));
	mBoard.PerformTurn(move, mPlayer);
	mGame->CallbackGotMove(this, move);
}