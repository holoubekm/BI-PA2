#include "caiminimax.h"
#include "cdesk.h"

/**
* Constructor
* @param maxLayer determines maximum depth of the recursion.
*/
CAIMinimax::CAIMinimax(int maxLayer)
{
	mMaxLayer = maxLayer;
	//mCalledCount = 0;
}

/**
* Helper method to call recursive solver
* @param board reference to the game board
* @param player determines player, whose move we want to solve
* @return returns the computed move
*/
CMove CAIMinimax::Solve(const CDesk& board, CSquare player)
{
	CDesk buf = CDesk(board);
	return Solve(buf, 0, player);
}

/**
* Recursive solver method
* @param board reference to the game board
* @param curLayer helper param, to check current depth of the recursion
* @param curPlayer determines player, whose move we want to solve
* @return returns the current move
*/
CMove CAIMinimax::Solve(CDesk& board, int curLayer, CSquare curPlayer)
{
	CMove bestMove;
	if(curLayer >= mMaxLayer)
	{
		return CMove(0, 0, 0);
	}

	CSquare nextPlayer;
	if(curPlayer == mBlack)
		nextPlayer = mWhite;
	else
		nextPlayer = mBlack;

	int possible = 0;
	int cur = 0;

	set<CMove, CMove>::iterator beg;
	set<CMove, CMove>::iterator end = board.mPosMoves.end();
	for(beg = board.mPosMoves.begin(); beg != end; ++beg)
	{
		CMove move = *beg;
		CDesk temp(board);
		move.mPrice = cur = temp.PerformTurn(move.mRow, move.mCol, curPlayer);

		if(cur > 0)
		{
			cur -= Solve(temp, curLayer + 1, nextPlayer).mPrice;	

			if(cur > bestMove.mPrice || (bestMove.mPrice == CMove::mMinPrice && move.mPrice > 0))
			{
				bestMove = move;
				bestMove.mPrice = move.mPrice;
			}
			possible++;
		}
	}

	if(curLayer == 0 && possible > 0)
	{
		bestMove.mPossible = true;
	}

	if(bestMove.mPrice < 0) bestMove.mPrice -= CMove::mMinPrice;

	return bestMove;
}
