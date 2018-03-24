#pragma once

#include <string>
#include <vector>

#include "iplayer.h"
#include "cnetwork.h"
#include "types.h"

using namespace std;

/**
* Client class, with own game board it represents the remote player
*
* @author Martin Holoubek
*/
class CClient : public IPlayer, public CNetwork
{
public:
	CClient(IGame* game, const CDesk& board, CSquare player);
	bool NextFrame();
	void SetOppMove(const CMove& move);
private:
	void ParseMessage(string message);
	vector<string> Split(string message, char delim);
};
