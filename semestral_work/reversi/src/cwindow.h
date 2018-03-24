#pragma once

#include <curses.h>
#include <string>
#include <vector>

#include "types.h"

using namespace std;

class CDesk;
class CGUI;

/**
* The main gui wrapper, entrance gate for all user input/output requests.
* Directly call method of the inner CGUI instance
*
* @author Martin Holoubek
*/
class CWindow
{
public:
	CWindow();
	~CWindow();
	void ShowWelcome();
	void ShowWinner(int white, int black);
	void ShowNetworkError();
	GameType SelectGameType();
	bool SelectHostType();
	bool SelectBotMode();
	PlayerType SelectPlayerType(CSquare player);
	void RenderBoard(const CDesk& board);
	void PrintScore(int white, int black);
	void PrintAddresses(const vector<string>& addresses);
	string GetHostIP();
	CMove GetMove(CSquare player);
public:
	static const int mMaxNameLen = 10;
	static const int mMaxIPLen = 15;
private:
	CGUI* mGui;
private:
};
