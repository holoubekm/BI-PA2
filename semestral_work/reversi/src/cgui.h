#pragma once

#include <string>
#include <vector>

#include "types.h"

using namespace std;

class CDesk;

/**
* Main GUI class, degined to provide simple interface to communicate with the unix terminal emulator
*
* @author Martin Holoubek
*/
class CGUI
{
public:
	CGUI();
	~CGUI();
	int ShowMenu(const string& label, const vector<string>& options);
	void ShowWithLabel(const string& label, const vector<string>& options);
	string GetStringAt(int length, int xPos, int yPos);
	string GetStringWidthLabel(const string& label, int length);
	CMove GetMoveWithLabel(const string& label);
	void PrintBanner(const string& text, int width, int top);
	void RenderBoard(const CDesk& board);
	void PrintText(const string& text, int xPos, int yPos);
	void PrintCentered(const string& text);
	void PrintScore(int white, int black);
private:
	void Refresh();
	void MoveCursor(int xPos, int yPos);
	void Clean(int xPos, int yPos, int width, int height);
	void CleanInput();
private:
	int mTermWidth;
	int mTermHeight;
};
