#include <iostream>
#include <iomanip>

#include "cdesk.h"

/**
* Constructor, sets up the start values and performs 4 fundamental moves
*/
CDesk::CDesk()
{
	Clear();
}

/**
* Copy constructor
* @param old source of the data
*/
CDesk::CDesk(const CDesk& old)
{
	for(int x = 0; x < mSize; x++)
	{
		for(int y = 0; y < mSize; y++)
		{
			mDesk[x][y] = old.mDesk[x][y];
		}
	}

	mWhiteCnt = old.mWhiteCnt;
	mBlackCnt = old.mBlackCnt;
	mOccupCnt = old.mOccupCnt;
	mFreeCnt = old.mFreeCnt;
	mPosMoves = old.mPosMoves;
}

/**
* Clear method reverts board to the fresh clean state
*/
void CDesk::Clear()
{
	for(int x = 0; x < mSize; x++)
	{
		for(int y = 0; y < mSize; y++)
		{
			mDesk[x][y] = mEmpty;
		}
	}

	PerformTurn(3, 3, mWhite);
	PerformTurn(3, 4, mBlack);
	PerformTurn(4, 3, mBlack);
	PerformTurn(4, 4, mWhite);
	
	mWhiteCnt = 2;
	mBlackCnt = 2;
	mOccupCnt = 4;
	mFreeCnt = 60;
}

/**
* Checks whether selected position is free
* @param row selects the current row
* @param col selects the current column
* @return determines emptiness of the position
*/
bool CDesk::IsFree(int row, int col) const
{
	return mDesk[row][col] == mEmpty;
}

/**
* Checks if the position is being occupied by the nemesis
* @param row selects the current row
* @param col selects the current column
* @param sq is the enemy's color
* @return returns 1 in the case of the opponent's tile, otherwise returns 0
*/
int CDesk::IsEnemy(int row, int col, CSquare sq) const
{
	if(mDesk[row][col] != sq && mDesk[row][col] != mEmpty)
	{
		return 1;
	}

	return 0;
}

/**
* Evaluates turn in the direction specified by pointers to function to avoid duplication
* @param row selects the current row
* @param col selects the current column
* @param rowDir is the pointer to function, which determines the row direction
* @param colDir is the pointer to function, which determines the column direction
* @param sq is the enemy's color
* @param perf selected whether the turn should be perfomed
* @return returns total number of the obtained tiles
*/
int CDesk::EvalInDir(int row, int col, int(*rowDir)(int), int(*colDir)(int), CSquare sq, bool perf = false)
{
	int score = 0;
	int y = colDir(col), x = rowDir(row);
	while(x < mSize && x >= 0 && y >= 0 && y < mSize)
	{
		score += IsEnemy(x, y, sq);
		if(mDesk[x][y] == sq)
		{
			return score;
		} else if(mDesk[x][y] == mEmpty) return 0;
		if(perf) mDesk[x][y] = sq;
		x = rowDir(x);
		y = colDir(y);
	} return 0;
	return 0;
}

/**
* Evaluates turn within the specified direction
* @param row selects the current row
* @param col selects the current column
* @param sq is the enemy's color
* @param perf selected whether the turn should be perfomed
* @return returns total number of the obtained tiles
*/
int CDesk::EvalTurn(int row, int col, CSquare sq, bool perf = false)
{
	//Critical logical section, may need to optimize for speed
	//Lambda functions assigned to function pointers to express the direction we want to check
	int(*right)(int) 	= [](int x){ return x + 1; };
	int(*left)(int)		= [](int x){ return x - 1; };
	int(*stay)(int) 	= [](int x){ return x; };
	int(*up)(int) 		= left;
	int(*down)(int) 	= right;

	int total = 0;
	int value;
	total += value = EvalInDir(row, col, stay, right, sq);
	if(value != 0 && perf) EvalInDir(row, col, stay, right, sq, perf);
	
	total += value = EvalInDir(row, col, stay, left, sq);
	if(value != 0 && perf) EvalInDir(row, col, stay, left, sq, perf);
	
	total += value = EvalInDir(row, col, up, stay, sq);
	if(value != 0 && perf) EvalInDir(row, col, up, stay, sq, perf);

	total += value = EvalInDir(row, col, down, stay, sq);
	if(value != 0 && perf) EvalInDir(row, col, down, stay, sq, perf);

	total += value = EvalInDir(row, col, down, right, sq);
	if(value != 0 && perf) EvalInDir(row, col, down, right, sq, perf);

	total += value = EvalInDir(row, col, up, right, sq);
	if(value != 0 && perf) EvalInDir(row, col, up, right, sq, perf);

	total += value = EvalInDir(row, col, up, left, sq);
	if(value != 0 && perf) EvalInDir(row, col, up, left, sq, perf);

	total += value = EvalInDir(row, col, down, left, sq);
	if(value != 0 && perf) EvalInDir(row, col, down, left, sq, perf);

	return total;
}

/**
* Performs turn within the specified direction
* @param row selects the current row
* @param col selects the current column
* @param sq is the enemy's color
* @return returns total number of the obtained tiles
*/
int CDesk::PerformTurn(int row, int col, CSquare sq)
{
	mDesk[row][col] = sq;
	if(row - 1 >= 0 && mDesk[row - 1][col] == mEmpty) 
		mPosMoves.insert(CMove(row - 1, col));
	if(row - 1 >= 0 && col + 1 < mSize && mDesk[row - 1][col + 1] == mEmpty) 
		mPosMoves.insert(CMove(row - 1, col + 1));
	if(col + 1 < mSize && mDesk[row][col + 1] == mEmpty) 
		mPosMoves.insert(CMove(row, col + 1));
	if(row + 1 < mSize && col + 1 < mSize && mDesk[row + 1][col + 1] == mEmpty) 
		mPosMoves.insert(CMove(row + 1, col + 1));
	if(row + 1 < mSize && mDesk[row + 1][col] == mEmpty) 
		mPosMoves.insert(CMove(row + 1, col));
	if(row + 1 < mSize && col - 1 >= 0 && mDesk[row + 1][col - 1] == mEmpty) 
		mPosMoves.insert(CMove(row + 1, col - 1));
	if(col - 1 >= 0 && mDesk[row][col - 1] == mEmpty) 
		mPosMoves.insert(CMove(row, col - 1));
	if(row - 1 >= 0 && col - 1 >= 0 && mDesk[row - 1][col - 1] == mEmpty) 
		mPosMoves.insert(CMove(row - 1, col - 1));
	mPosMoves.erase(CMove(row, col));
	
	int price = EvalTurn(row, col, sq, true);

	if(sq == mWhite)
	{
		mWhiteCnt += price + 1;
		mBlackCnt -= price;
	}
	else
	{
		mWhiteCnt -= price;
		mBlackCnt += price + 1;
	}
	mOccupCnt++;
	mFreeCnt--;

	return price;
}

/**
* Performs turn within the specified direction
* @param move move to be perfomed
* @param sq is the enemy's color
* @return returns total number of the obtained tiless
*/
int CDesk::PerformTurn(const CMove& move, CSquare sq)
{
	return PerformTurn(move.mRow, move.mCol, sq);
}

/**
* Prints the board to the stdout, for debugging purposes only
*/
void CDesk::Print() const
{
	cout << ' ';
	for(int x = 0; x < mSize; x++)
	{
		cout << setw(2) << x;
	}
	cout << endl;

	for(int x = 0; x < mSize; x++)
	{
		cout << x;
		for(int y = 0; y < mSize; y++)
		{
			cout << " ";
			if(mDesk[x][y] == mWhite) cout << 'w';
			else if(mDesk[x][y] == mBlack) cout << 'b';
			else cout << mDesk[x][y];
		}
		cout << endl;
	}
}

/**
* Prints the possible move to the stdout, for debugging purposes only
*/
void CDesk::PrintMoves() const
{
	cout << ' ';
	for(int x = 0; x < mSize; x++)
	{
		cout << /*setw(2) << */ ' ' <<x;
	}
	cout << endl;

	for(int x = 0; x < mSize; x++)
	{
		cout << x;
		for(int y = 0; y < mSize; y++)
		{
			cout << ' ';
			if(mPosMoves.count(CMove(x, y)) != 0)
				cout << 1;
			else if(mDesk[x][y] == mWhite) cout << 'w';
			else if(mDesk[x][y] == mBlack) cout << 'b';
			else cout << ' ';
		}
		cout << endl;
	}
}