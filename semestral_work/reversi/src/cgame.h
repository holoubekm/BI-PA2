#pragma once

#include <string>

#include "types.h"
#include "igame.h"

using namespace std;

class CDesk;
class CWindow;
class IPlayer;

/**
 * The main game class, handles basic game events
 *
 * @author Martin Holoubek
 */
class CGame : private IGame
{
public:
	CGame();
	~CGame();
	bool Start();

	void GetOppMove(IPlayer* player);
	void CallbackGotMove(const IPlayer* player, const CMove move);
public:
	const string mLocalIP;
private:
	bool StartNetworkGame();
	bool StartLocalGame();
	void GameLoop();
private:
	CWindow* mWindow;

	IPlayer* mPlayerA;
	IPlayer* mPlayerB;

	CDesk* mBoard;

	GameType mType;
	GameStatus mStatus;
	GameController mController;
	bool mIsHost;
	bool mBusy;
	string mHostIP;
};
