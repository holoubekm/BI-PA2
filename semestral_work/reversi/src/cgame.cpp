#include "cwindow.h"
#include "cnetwork.h"
#include "cdesk.h"
#include "chuman.h"
#include "cbot.h"
#include "cclient.h"
#include "cserver.h"

#include <unistd.h>

#include "cgame.h"

/**
* The initializing constructor
*/
CGame::CGame()
{
	mType = mGameUndefined;
	mStatus = mStatusInit;
	mController = mUnknown;
	mBusy = false;
	mBoard = new CDesk();
	mWindow = new CWindow();
	mPlayerA = NULL;
	mPlayerB = NULL;
}

/**
* Main game entrace method, 
* it blocks main thread in infinity game loop.
* Method shows welcome screen and diplay dialog box.
* @return determines success or fail
*/
bool CGame::Start()
{
	bool result = true;
	mWindow->ShowWelcome();
	mType = mWindow->SelectGameType();
	switch(mType)
	{
		case mGameEnd:
		return true;
		case mGameNetwork:
			result = StartNetworkGame();
			break;
		case mGameLocal:
			result = StartLocalGame();
			break;
		default:
			return false;
	}
	if(!result) return false;
	GameLoop();
	return true;
}

/**
* Helper method to start a network game. 
* It shows dialog box to set up game.
* @return determines success or fail
*/
bool CGame::StartNetworkGame()
{
	mIsHost = mWindow->SelectHostType();
	if(mIsHost)
	{
		mPlayerA = new CHuman(this, mWindow, *mBoard, mWhite);
		mPlayerB = new CServer(this, *mBoard, mBlack);
		mWindow->PrintAddresses(((CServer*)mPlayerB)->GetAddresses());
		if(!((CServer*)mPlayerB)->AwaitConnection())
		{
			mWindow->ShowNetworkError();
			return false;
		}
		mBusy = true;
	}
	else
	{
		do
		{
			mHostIP = mWindow->GetHostIP();
		}
		while(!CNetwork::ParseIP(mHostIP));

		mPlayerB = new CHuman(this, mWindow, *mBoard, mBlack);
		mPlayerA = new CClient(this, *mBoard, mWhite);
		if(!((CClient*)mPlayerA)->Connect(mHostIP))
		{ 
			mWindow->ShowNetworkError();
			return false;
		}
		mBusy = false;
	}
	return true;
}

/**
* Helper method to start a local game
* @return determines success or fail
*/
bool CGame::StartLocalGame()
{
	PlayerType pType = mWindow->SelectPlayerType(mWhite);
	switch(pType)
	{
		case PlayerType::mGameEnd:
			return false;
		case PlayerType::mHuman:
			mPlayerA = new CHuman(this, mWindow, *mBoard, mWhite);
			break;
		case PlayerType::mBot:
			bool hard = mWindow->SelectBotMode();
			mPlayerA = new CBot(this, *mBoard, mWhite, hard);
			break;
	}

	pType = mWindow->SelectPlayerType(mBlack);
	switch(pType)
	{
		case PlayerType::mGameEnd:
			return false;
		case PlayerType::mHuman:
			mPlayerB = new CHuman(this, mWindow, *mBoard, mBlack);
			break;
		case PlayerType::mBot:
			bool hard = mWindow->SelectBotMode();
			mPlayerB = new CBot(this, *mBoard, mBlack, hard);
			break;
	}
	mBusy = true;
	return true;
}

/**
* Infinity game loop method, which hanles game events and distribute commands to IPlayer classes
*/
void CGame::GameLoop()
{
	mWindow->RenderBoard(*mBoard);
	mWindow->PrintScore(mBoard->mWhiteCnt, mBoard->mBlackCnt);
	mController = mGetMoveA;

	mStatus = mStatusRunning;
	//Main game loop
	while(mStatus == mStatusRunning)
	{
		if(mBusy)
		{
			mBusy = false;
			if(!mPlayerA->CanPlay() && !mPlayerB->CanPlay())
			{
				mStatus = mStatusFinished;
				break;
			}

			switch(mController)
			{
				case mGetMoveA:
					mPlayerA->GetMoveAsync();
					break;
				case mGetMoveB:
					mPlayerB->GetMoveAsync();
					break;
				default:
					return;
			}
		}
		if(!mPlayerA->NextFrame() || !mPlayerB->NextFrame())
		{
			mStatus = mStatusNetworkError;
		}
		//Slow-down mechanism to keep the CPU's freq low
		usleep(1000 * CHECK_TIME_MS);
	}

	switch(mStatus)
	{
		case mStatusFinished:
			mWindow->ShowWinner(mBoard->mWhiteCnt, mBoard->mBlackCnt);
			break;
		case mStatusNetworkError:
			mWindow->ShowNetworkError();
			break;
		default:
			return;
	}
}

/**
* Callback method, which is invoked directly by the player to perform new turn
*/
void CGame::CallbackGotMove(const IPlayer* player, const CMove move)
{
	if(player == mPlayerA)
	{
		mPlayerB->SetOppMove(move);
		mController = mPlayerB->CanPlay() ? mGetMoveB : mGetMoveA;
	}
	else
	{
		mPlayerA->SetOppMove(move);
		mController = mPlayerA->CanPlay() ? mGetMoveA : mGetMoveB;
	}

	mBoard->PerformTurn(move.mRow, move.mCol, player->mPlayer);
	mWindow->RenderBoard(*mBoard);
	mWindow->PrintScore(mBoard->mWhiteCnt, mBoard->mBlackCnt);

	if(dynamic_cast<CClient*>(mPlayerA) == NULL)
	{
		mBusy = true;
	}
}

/**
* Callback method, useful during network game to ensure server's supremacy
*/
void CGame::GetOppMove(IPlayer* player)
{
	if(player == mPlayerA)
	{
		mController = mPlayerB->CanPlay() ? mGetMoveB : mGetMoveA;
	}
	else
	{
		mController = mPlayerA->CanPlay() ? mGetMoveA : mGetMoveB;
	}
	mBusy = true;
}

/**
* Destructor, it takes care about desctructing game objects in a safe way
*/
CGame::~CGame()
{
	DeleteObject(mBoard);
	DeleteObject(mWindow);
	DeleteObject(mPlayerA);
	DeleteObject(mPlayerB);
}
