#include <sstream>

#include "igame.h"
#include "cdesk.h"
#include "cwindow.h"

#include "cclient.h"

/**
* Constructor
* @param game pointer to main game class to make callbacks possible
* @param board reference to CDesk instance, players gameboard
* @param player player's color
*/
CClient::CClient(IGame* game, const CDesk& board, CSquare player) : IPlayer(game, board, player)
{ 
}

/**
* Overridden method, tries to receive some data from server, in the case of succession calls ParseMessage() method
* @return network state
*/
bool CClient::NextFrame()
{
	string message;
	switch(TryReceive(message))
	{
		case mNetworkTrue:
			ParseMessage(message);
			break;
		case mNetworkError:
			return false;
		default:
			break;
	}
	return true;
}

/**
* Helper method to split string with a delimiter
* @return chunks stored in the vector
*/
vector<string> CClient::Split(string message, char delim = ':')
{
	stringstream input(message);
	string item;
	vector<string> elems;
	while (getline(input, item, delim)) 
	{
        elems.push_back(item);
    }
    return elems;
}

/**
* Helper method to parse received message
*/
void CClient::ParseMessage(string message)
{
	vector<string> data = Split(message);
	if(data.size() > 1 && data[0] == "set")
	{
		string cmd = data[1];
		if(cmd == "move")
		{
			CMove move(0, 0, 0);
			istringstream row(data[2]);
			istringstream col(data[3]);
			row >> move.mRow;
			col >> move.mCol;
			mBoard.PerformTurn(move, mPlayer);
			mGame->CallbackGotMove(this, move);
			mGame->GetOppMove(this);
		}
	}
}

/**
* Method to set opponents move, also send move's data to the remove server
*/
void CClient::SetOppMove(const CMove& move)
{
	IPlayer::SetOppMove(move);
	ostringstream message;
	message << "set:move:" << move.mRow << ":" << move.mCol;
	Send(message.str());
}
