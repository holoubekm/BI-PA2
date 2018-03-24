#pragma once

#include "types.h"
#include "cdesk.h"

class IGame;
class CDesk;

/**
* Abstract class, provides some useful methods, while the virtual ones should be overridden
*
* @author Martin Holoubek
*/
class IPlayer
{
public:
	IPlayer(IGame* game, const CDesk& board, CSquare player);
	virtual ~IPlayer(){};
	virtual bool NextFrame();
	virtual void GetMoveAsync();
	virtual void SetOppMove(const CMove& move);
	bool CanPlay();
public:
	CSquare mPlayer;
protected:
	bool AllowedMove(const CMove& move);
	CSquare GetOpponent();
protected:
	IGame* mGame;
	CDesk mBoard;
};
