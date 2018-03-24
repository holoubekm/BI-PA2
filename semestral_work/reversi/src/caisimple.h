#pragma once

#include "iaibase.h"

class CDesk;
class CMove;

/**
 * The minimax game class, it provides basic AI method
 *
 * @author Martin Holoubek
 */
class CAISimple : public IAIBase
{
public:
	CMove Solve(const CDesk& board, CSquare player);
};

