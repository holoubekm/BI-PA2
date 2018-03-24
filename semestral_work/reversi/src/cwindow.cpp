#include <sstream>
#include <unistd.h>

#include "cgui.h"
#include "cdesk.h"
#include "cwindow.h"


/**
* Constructor
*/
CWindow::CWindow()
{
	mGui = new CGUI();
}

/**
* Destructor, delete objects in the safe way
*/
CWindow::~CWindow()
{
	DeleteObject(mGui);
}

/**
* GameType selection method
* @return selected game type
*/
GameType CWindow::SelectGameType()
{
	string label = "Select one of the following: ";
	vector<string> options;
	options.push_back("Local game");
	options.push_back("Network game");
	options.push_back("Exit game");

	int result = mGui->ShowMenu(label, options);
	return (GameType) result;
}

/**
* PlayerType selection method
* @param player color of the player, whose type we are choosing
* @return selected player type
*/
PlayerType CWindow::SelectPlayerType(CSquare player)
{
	ostringstream label;
	label << "Select " << (player == mWhite ? "white" : "black") << " the player: ";
	vector<string> options;
	options.push_back("Human");
	options.push_back("Bot");
	options.push_back("Exit game");

	int result = mGui->ShowMenu(label.str(), options);
	return (PlayerType) result;
}

/**
* HostType selection method
* @return bool value corresponding to the host type
*/
bool CWindow::SelectHostType()
{
	string label = "I will play as: ";
	vector<string> options;
	options.push_back("Host");
	options.push_back("Client");

	int result = mGui->ShowMenu(label, options);
	return !(bool)result;
}

/**
* Selects AI skill options
* @return positive value means harder opponent
*/
bool CWindow::SelectBotMode()
{
	string label = "Select computer skill level: ";
	vector<string> options;
	options.push_back("Lower");
	options.push_back("Higher");

	int result = mGui->ShowMenu(label, options);
	return !(bool)result;
}

/**
* Gameboard rendering method
* @param board to be rendered
*/
void CWindow::RenderBoard(const CDesk& board)
{
	mGui->RenderBoard(board);
}

/**
* Writes the both players score
* @param white current score of the white player
* @param black current score of the black player
*/
void CWindow::PrintScore(int white, int black)
{
	mGui->PrintScore(white, black);
}

/**
* Prints enumerated ipv4 addresses to the screen, ussualy being shown while waiting for a client
* @param addresses vector of strings to be printed out
*/
void CWindow::PrintAddresses(const vector<string>& addresses)
{
	string label = "Server listening at: ";
	mGui->ShowWithLabel(label, addresses);
}

/**
* Method to provide client with a server's ip address
* @return the ip address
*/
string CWindow::GetHostIP()
{
	string label = "Insert the host's IP adress: "; 
	return mGui->GetStringWidthLabel(label, 15);
}

/**
* Method to load next move from the terminal
* @param player we want to get next move for
* @return next move
*/
CMove CWindow::GetMove(CSquare player)
{
	ostringstream label;
	label << (player == mWhite ? "white" : "black") << " at: ";
	return mGui->GetMoveWithLabel(label.str());
}

/**
* Prints the network error dialogn, blocking method
*/
void CWindow::ShowNetworkError()
{
	string label = "External network exception cought, the game will be terminated!";
	mGui->PrintCentered(label);
	usleep(1000 * SLEEP_TIME_MS);
}

/**
* Diplays the welcome screen
*/
void CWindow::ShowWelcome()
{
	string label;
	label += "   _____                           _    \n";
	label += "  | __  | ___  _ _  ___  ___  ___ |_|   \n";
	label += "  |    -|| -_|| | || -_||  _||_ -|| |   \n";
	label += "  |__|__||___| \\_/ |___||_|  |___||_|   \n";
	label += "           ___  ___  ___    ___         \n";
	label += "          |_  ||   ||_  |  | | |        \n";
	label += "          |  _|| | | _| |_ |_  |        \n";
	label += "          |___||___||_____|  |_|        \n";
	label += " _____       _            _         _   \n";
	label += "|  |  | ___ | | ___  _ _ | |_  ___ | |_ \n";
	label += "|     || . || || . || | || . || -_|| '_|\n";
	label += "|__|__||___||_||___||___||___||___||_,_|\n";
	label += "                                        \n";
	mGui->PrintBanner(label, 40, 6);
}

/**
* Diplays the winner screen
* @param white score of the player
* @param black score of player
*/
void CWindow::ShowWinner(int white, int black)
{
	string label;
	int width = 0;
	if(white == black)
	{
		label += "                        \n";
		label += " ____                   \n";
		label += "|    \\  ___  ___  _ _ _ \n";
		label += "|  |  ||  _|| .'|| | | |\n";
		label += "|____/ |_|  |__,||_____|\n";
		label += "                        \n";
		width = 24;
	}
	else if(white > black)
	{
		label += " _ _ _  _    _  _        \n";
		label += "| | | || |_ |_|| |_  ___ \n";
		label += "| | | ||   || ||  _|| -_|\n";
		label += "|_____||_|_||_||_|  |___|\n";
		label += "   _ _ _  _              \n";
		label += "  | | | ||_| ___  ___    \n";
		label += "  | | | || ||   ||_ -|   \n";
		label += "  |_____||_||_|_||___|   \n";
		label += "                         \n";
		width = 25;
	}
	else
	{
		label += " _____  _            _   \n";
		label += "| __  || | ___  ___ | |_ \n";
		label += "| __ -|| || .'||  _|| '_|\n";
		label += "|_____||_||__,||___||_,_|\n";
		label += "   _ _ _  _              \n";
		label += "  | | | ||_| ___  ___    \n";
		label += "  | | | || ||   ||_ -|   \n";
		label += "  |_____||_||_|_||___|   \n";
		label += "                         \n";
		width = 25;
	}
	mGui->PrintBanner(label, width, 6);
}