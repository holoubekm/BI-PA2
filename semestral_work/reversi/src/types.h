#pragma once

#define DEBUG

#define DeleteObject(a) if(a != NULL) { delete (a); (a) = NULL; }
#define DeleteArray(a) if(a != NULL) { delete[] (a); (a) = NULL; }

#include "debugger/cdebug.h"

#define PORT "45678"
#define BUF_LEN 128
#define CHECK_TIME_MS 500
#define SLEEP_TIME_MS 5000

/**
* Screen colors enum
*/
enum Colors
{
	mColWindow = 1,
	mColText,
	mColBanner,
	mColDeskW,
	mColDeskB
};

/**
* GameType enum to determine current game type
*/
enum GameType
{
	mGameLocal,
	mGameNetwork,
	mGameEnd,

	mGameUndefined
};

/**
* GameStatus enum for the game state machine
*/
enum GameStatus
{
	mStatusInit,
	mStatusRunning,
	mStatusFinished,
	mStatusNetworkError
};

/**
* NetworkStatus enum, return value of the networking methods to determine connection state
*/
enum NetworkStatus
{
	mNetworkTrue,
	mNetworkFalse,
	mNetworkError
};

/**
* Helper enum which holds the next action in the game loop
*/
enum GameController
{
	mGetMoveA,
	mGetMoveB,
	mUnknown
};

/**
* PlayerType to distinguish possible IPlayer children
*/
enum class PlayerType
{
	mHuman,
	mBot,
	mGameEnd
};

/**
* CSquare enum which determines player color and fills up the game board
* Default values have been changed for debugging purposes
*/
enum CSquare
{
	mEmpty = 0,
	mWhite = 20,
	mBlack
};

/**
* Wrapper for move, it indicated positions, value and possibility of the current move.
* The meaning may be different in the contexts
*
* @author Martin Holoubek
*/
class CMove
{
public:
	CMove(short row = 0, short col = 0, short price = mMinPrice, bool possible = true) 
	: mRow(row), mCol(col), mPrice(price), mPossible(possible) {}
public:
	short mRow;
	short mCol;
	short mPrice;
	static const short mMinPrice = -64;
	bool mPossible;

	bool operator()(const CMove& lhs, const CMove& rhs) const
    {
        if(lhs.mRow < rhs.mRow)
        	return true;
        else if(lhs.mRow == rhs.mRow)
        	return lhs.mCol < rhs.mCol;
        return false;
    }
};

