#pragma once

#include "types.h"

class IPlayer;

/**
* Game interface to supply players with the callback methods without access to all game fields
*
* @author Martin Holoubek
*/
class IGame
{
public:
	virtual ~IGame() {};
	virtual void GetOppMove(IPlayer* player) = 0;
	virtual void CallbackGotMove(const IPlayer* player, const CMove move) = 0;
};
