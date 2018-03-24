#include "caisimple.h"
#include "cdesk.h"

/**
* Linear solver method, provides result in a short time
* @param board reference to the game board
* @param player determines player, whose move we want to solve
* @return returns the computed move
*/
CMove CAISimple::Solve(const CDesk& board, CSquare player)
{
	//cout << "Simple called" << endl;
	set<CMove, CMove>::iterator beg;
	set<CMove, CMove>::iterator end = board.mPosMoves.end();

	CMove bestMove;

	for(beg = board.mPosMoves.begin(); beg != end; ++beg)
	{
		CMove move = *beg;
		CDesk temp(board);
		move.mPrice = temp.PerformTurn(move.mRow, move.mCol, player);

		if(move.mPrice > 0 && move.mPrice > bestMove.mPrice)
		{
			bestMove = move;
		}
	}

	return bestMove;
}