#pragma once

#include "iaibase.h"

using namespace std;

class CDesk;
class CMove;

/**
 * The minimax game class, it provides extended AI methods
 *
 * @author Martin Holoubek
 */
class CAIMinimax : public IAIBase
{
public:
	CAIMinimax(int maxLayer = 5);
	CMove Solve(const CDesk& board, CSquare player);
private:
	CMove Solve(CDesk& board, int curLayer, CSquare curPlayer);
private:
	int mMaxLayer;
};

