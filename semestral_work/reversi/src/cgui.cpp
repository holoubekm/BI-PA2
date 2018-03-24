#include <curses.h>
#include <string>
#include <unistd.h>
#include <cctype>
#include <algorithm>
#include <vector>
#include <sstream>
#include <cctype>

#include "cdesk.h"
#include "cgui.h"

using namespace std;

/**
* Constructor, initilizes variables, ncurses and defines own interface colors.
* Prescribed library ncurses leads to memory leaks, which can be repaired with recompilation.
*/
CGUI::CGUI()
{
	initscr();
	getmaxyx(stdscr, mTermHeight, mTermWidth);

	start_color();

	init_pair(mColWindow, COLOR_WHITE, COLOR_BLACK);
	init_pair(mColText, COLOR_CYAN, COLOR_BLACK);
	init_pair(mColBanner, COLOR_CYAN, COLOR_BLACK);
	init_pair(mColDeskW, COLOR_WHITE, COLOR_BLUE);
	init_pair(mColDeskB, COLOR_BLACK, COLOR_BLUE);

	wattron(stdscr, COLOR_PAIR(mColWindow));
	wborder(stdscr, 0, 0, 0, 0, 0, 0, 0, 0);
	wrefresh(stdscr);
}

/**
* Destructor, releases the window handle, doesn't free library memory at all
*/
CGUI::~CGUI()
{
	endwin();
}

/**
* Shows select box with label and options, blocking method
* @param label is the text label places above the options' list
* @param options is the vector of the choices
* @return zero based selection value
*/
int CGUI::ShowMenu(const string& label, const vector<string>& options)
{
	curs_set(0);
	int top = (mTermHeight - 4) / 2;;
	int left = (mTermWidth - label.size()) / 2;
	int count = options.size();
	wmove(stdscr, top + 1, left);
	wattron(stdscr, COLOR_PAIR(mColText) | A_BOLD);
	addstr(label.c_str());

	int width = label.size();
	wattron(stdscr, COLOR_PAIR(mColWindow));

	int x = 0;
	for_each(options.begin(), options.end(), [&](string value)
	{
		ostringstream text;
		text << "[" << ((char)('0' + x)) << "]: " << value;
		wmove(stdscr, top + x + 2, left + 4); addstr(text.str().c_str());
		width = label.size() > (uint)width ? label.size() : width;
		x++;
	});

	wrefresh(stdscr);
	CleanInput();

	int result = -1;
	bool loop = true;
	while(loop)
	{
		int cur = getch();
		if(cur >= '0' && cur < '0' + count)
		{
			loop = false;
			result = cur;
		}
	}

	wattron(stdscr, COLOR_PAIR(mColWindow));
	Clean(left, top, width, options.size() + 3);
	return result - '0';
}

/**
* Prints labeled strings in one column
* @param label is the text label places above the options' list
* @param options is the vector of the choices
*/
void CGUI::ShowWithLabel(const string& label, const vector<string>& options)
{
	curs_set(0);
	int top = (mTermHeight - 4) / 2;;
	int left = (mTermWidth - label.size()) / 2;
	wmove(stdscr, top + 1, left);
	wattron(stdscr, COLOR_PAIR(mColText) | A_BOLD);
	addstr(label.c_str());

	int width = label.size();
	wattron(stdscr, COLOR_PAIR(mColWindow));

	int x = 0;
	for_each(options.begin(), options.end(), [&](string value)
	{
		wmove(stdscr, top + x + 2, left + 4); addstr(value.c_str());
		width = label.size() > (uint)width ? label.size() : width;
		x++;
	});

	wrefresh(stdscr);
}

/**
* Method to load user input with label, blocking method
* @param label is the text places above the input
* @param length is maximum length of the input
* @return user input
*/
string CGUI::GetStringWidthLabel(const string& label, int length)
{
	curs_set(0);
	int top = (mTermHeight - 2) / 2;;
	int left = (mTermWidth - label.size()) / 2;
	int width = label.size() < (uint)length ? length : label.size();

	wmove(stdscr, top, left);
	wattron(stdscr, COLOR_PAIR(mColText) | A_BOLD);
	addstr(label.c_str());

	int sleft = (mTermWidth - length) / 2;
	string output = GetStringAt(length, sleft, top + 2);

	wattron(stdscr, COLOR_PAIR(mColWindow));
	Clean(left, top, width, 3);
	return output;
}

/**
* Loads next move, with basic overflow check and label, blocking method
* @param label is the text label places above the user input
* @return next move
*/
CMove CGUI::GetMoveWithLabel(const string& label)
{
	int length = label.size();
	int xPos = 4;
	int yPos = 10;

	PrintText(label, xPos, yPos);
	string input;
	int row, col;
	do
	{
		input = GetStringAt(2, xPos + length, yPos);
		if(input.size() < 2) continue;
		row = toupper(input[0]) - 'A';
		col = input[1] - '0';
	}
	while(row < 0 || row >= 8 || col < 0 || col >= 8);
	Clean(xPos, yPos, length + 2, 1);
	return CMove(row, col);
}

/**
* Prints multiline, fullscreen banner, blocking method
* @param text consists of lines separated with the newline char
* @param width length of the longest line in the input text
* @param top is the top screen offset
*/
void CGUI::PrintBanner(const string& text, int width, int top)
{
	int xPos = 0;
	int yPos = 0;
	int left = (mTermWidth - width) / 2;

	clear();
	wattron(stdscr, COLOR_PAIR(mColWindow));
	wborder(stdscr, 0, 0, 0, 0, 0, 0, 0, 0);
	curs_set(0);
	for(int x = 0; x < (int)text.size(); x++)
	{
		int cur = text[x];
		if(cur == '\n')
		{
			yPos++;
			xPos = 0;
			continue;
		}
		mvwaddch(stdscr, yPos + top, xPos + left, cur | A_BOLD | COLOR_PAIR(mColBanner));
		xPos++;
	}
	wrefresh(stdscr);
	usleep(1000 * SLEEP_TIME_MS);
	Clean(left, top, text.size() / yPos, yPos);
}

/**
* Clean, fills the selected rectangle with the space char
* @param xPos denotes the vertical position
* @param yPos denotes the horizontal position
* @param width determine rectanle's width
* @param height determine rectanle's height
*/
void CGUI::Clean(int xPos, int yPos, int width, int height)
{
	for(int x = xPos; x < xPos + width; x++)
	{
		for(int y = yPos; y < yPos + height; y++)
		{
			mvwaddch(stdscr, y, x, ' ' | COLOR_PAIR(mColWindow));
		}
	}
	wrefresh(stdscr);
}

/**
* Loads the limited length string at the chosen position, blocking method
* @param length maximum allowable length of the input string
* @param xPos denotes the vertical position
* @param yPos denotes the horizontal
* @return user input
*/
string CGUI::GetStringAt(int length, int xPos, int yPos)
{
	curs_set(FALSE);
	CleanInput();
	keypad(stdscr, TRUE);
	wmove(stdscr, yPos, xPos);
	wattron(stdscr, COLOR_PAIR(mColWindow));
	length++;
	{ for(int x = 0; x < length - 1; x++) addch('_'); }
	string output;

	int x = 1;
	while(true)
	{
		int cur;
		do
		{
			cur = getch();
			if(cur == KEY_BACKSPACE)
			{
				if(x > 1) x--;
				wmove(stdscr, yPos, xPos + x - 1); addch('_');
				wmove(stdscr, yPos, xPos);
				output = output.substr(0, output.size() - 1);
				addstr(output.c_str());
			}
			else if((cur == KEY_ENTER || cur == '\n') && output.size() > 0)
			{
				keypad(stdscr, FALSE);
				return output;
			}
		} 
		while (!isalnum(cur) && !ispunct(cur));

		if(x < length)
		{
			output += cur;
			wmove(stdscr, yPos, xPos);
			addstr(output.c_str());
			x++;
		}
	}
	
	keypad(stdscr, FALSE);
	return output;
}

/**
* The main rendering method, prints out the game board
* @param board is actually an array of values to be printed
*/
void CGUI::RenderBoard(const CDesk& board)
{
	int xPos = (mTermWidth - 16) / 2;
	int yPos = 6;

	clear();
	wattron(stdscr, COLOR_PAIR(mColWindow));
	wborder(stdscr, 0, 0, 0, 0, 0, 0, 0, 0);

	for(int x = 0; x < board.mSize; x++)
	{
		mvwaddch(stdscr, yPos - 1, xPos + x * 2, ('0' + x) | COLOR_PAIR(mColWindow));
	}

	for(int y = 0; y < board.mSize; y++)
	{
		for(int x = -1; x < board.mSize; x++)
		{
			if(x == -1)
			{
				mvwaddch(stdscr, yPos + y, xPos + x * 2, ('A' + y) | COLOR_PAIR(mColWindow));
				continue;
			}

			char sign = '.';
			int color = mColDeskW;
			if(board.mDesk[y][x] == mWhite)
			{
				sign = 'W';
			}
			else if(board.mDesk[y][x] == mBlack)
			{
				sign = 'B';
				color = mColDeskB;
			}

			mvwaddch(stdscr, yPos + y, xPos + x * 2, sign | COLOR_PAIR(color));
			mvwaddch(stdscr, yPos + y, xPos + x * 2 + 1, ' ' | COLOR_PAIR(color));
				
		}
	}
	curs_set(FALSE);
	wrefresh(stdscr);
}

/**
* Prints text at specified positions
* @param text is the desired output
* @param xPos is the horizontal position
* @param yPos is the vertical position
*/
void CGUI::PrintText(const string& text, int xPos, int yPos)
{
	int size = text.size();
	for(int x = 0; x < size; x++)
	{
		char cur = text[x];
		mvwaddch(stdscr, yPos, xPos + x, cur | COLOR_PAIR(mColWindow));
	}
	wrefresh(stdscr);
}

/**
* Prints text at the center of the the screen
* @param text is the desired output
*/
void CGUI::PrintCentered(const string& text)
{
	int xPos = (mTermWidth - text.size()) / 2;
	int yPos = mTermHeight / 2;
	clear();
	wattron(stdscr, COLOR_PAIR(mColWindow));
	wborder(stdscr, 0, 0, 0, 0, 0, 0, 0, 0);
	PrintText(text, xPos, yPos);
}

/**
* Prints score to the top-left corner
* @param white is the score of the white player
* @param black is the score of the black player
*/
void CGUI::PrintScore(int white, int black)
{
	ostringstream scorew;
	scorew << "white: " << white;
	ostringstream scoreb;
	scoreb << "black: " << black;

	PrintText(scorew.str(), 4, 6);
	PrintText(scoreb.str(), 4, 7);
}

/**
* Clean input flags, inner method
*/
void CGUI::CleanInput()
{
	flushinp();
	noecho();
	cbreak();
}
