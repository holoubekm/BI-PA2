#pragma once

#include <set>

#include "types.h"

using namespace std;

/**
* CDesk class, the heart of the game, provides robust interface over the whole gameboard.
* It has ability to evaluate turns, check their possibility and even perform them on itself.
*
* @author Martin Holoubek
*/
class CDesk
{
public:
	CDesk();
	CDesk(const CDesk& old);
	int PerformTurn(const CMove& move, CSquare sq);
	int PerformTurn(int row, int col, CSquare sq);
	void Print() const;
	void PrintMoves() const;
private:
	void Clear();
	bool IsFree(int row, int col) const;
	int IsEnemy(int row, int colr, CSquare sq) const;
	int EvalInDir(int row, int col, int(*rowDir)(int), int(*colDir)(int), CSquare sq, bool perf);
	int EvalTurn(int row, int col, CSquare sq, bool perf);
	
public:
	short mFreeCnt;
	short mWhiteCnt;
	short mBlackCnt;
	short mOccupCnt;
	static const short mSize = 8;
	static const short mTotalCnt = 64;
	set<CMove, CMove> mPosMoves;
	char mDesk[mSize][mSize];
};

