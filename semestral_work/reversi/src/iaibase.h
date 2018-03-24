#pragma once

#include "types.h"

class CDesk;

/**
* Abstract class providing interface between AI classes and the CBot class
*
* @author Martin Holoubek
*/
class IAIBase
{
public:
	virtual CMove Solve(const CDesk& board, CSquare player) = 0;
	virtual ~IAIBase(){};
};
