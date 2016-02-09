#ifndef GUARD_board_h
#define GUARD_board_h

#include <string>

// board.h
class Board
{
public:
	Board(void);
	void jacobiUpdate();
	void writeBoard(std::string);
	void textInitialise(std::string);
};

#endif
