#define GUARD_board_h

#include <string>
#include <vector>

// board.h
class Board
{
public:

	std::vector < std::vector<std::tuple<double, bool>> > grid;
	std::vector<std::vector<std::tuple<double, bool>>> last_grid;
	bool converged(double);
	void jacobiUpdate();
	void gaussUpdate();
	void sorUpdate();
	void writeBoard(std::string, std::string);
	void bmpInitialise(std::string);
	
};
