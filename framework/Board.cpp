#include "stringmanip.h"
#include <tuple>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class Board
{
public:
	static const int GRID_WIDTH = 101;
	static const int GRID_HEIGHT = 101;
	Board(void); // default constructor
	Board(string file_name); // constructor with shapes file
	void jacobiUpdate();
	void writeBoard(string ofile_name);
	void textInitialise(string ifile_name);

	tuple<double, bool> grid[GRID_WIDTH][GRID_HEIGHT];

};

// Member function definitions:

/*****************************************************************************************************************************************/
// Default constructor:

Board::Board(void) {};


// Initialises a board from a text file
void Board::textInitialise(string ifilename) {

	ifstream shapes_file;
	shapes_file.open(ifilename);
	string s;
	// For each rectangle
	while (getline(shapes_file, s)){

		// Break the line into its constituent ints, representing the properties of the rectangle
		vector<int> properties = split(s);
		vector<int>::const_iterator iter = properties.begin();
		size_t origin_x = *iter;
		++iter;
		size_t origin_y = *iter;
		++iter;
		size_t width = *iter;
		++iter;
		size_t height = *iter;
		++iter;
		int potential = *iter;

		// Check if the rectangle will fit on the board
		if (origin_x < GRID_WIDTH && origin_x + width <= GRID_WIDTH && origin_y < GRID_HEIGHT && origin_y + height <= GRID_HEIGHT) {
			// Enter the given potential at every point on the grid that lies within the rectangle.
			// For each x-coord within the rectangle
			for (size_t i = origin_x; i < origin_x + width; ++i) {
				// For each y-coord within the rectangle
				for (size_t j = origin_y; j < origin_y + height; ++j) {
					// Set the potential of this cell.
					get<0>(grid[i][j]) = potential;
					// Mark this cell as permanent
					get<1>(grid[i][j]) = 1;
				}
			}
		}
	}
	shapes_file.close();
}


// Performs one iteration of the Jacobi method of relaxation. SLOW! ***************************************************************************************
void Board::jacobiUpdate() {

	// Make a copy of the grid
	tuple<double, bool> new_grid[GRID_WIDTH][GRID_HEIGHT];
	//copy(&grid[0][0], &grid[0][0] + GRID_WIDTH*GRID_HEIGHT, &new_grid[0][0]);
	for (size_t i = 0; i < GRID_WIDTH; ++i) {
		for (size_t j = 0; j < GRID_HEIGHT; ++j) {
			new_grid[i][j] = grid[i][j];
		}
	}
		
	// First update the corner pieces:

	if (get<1>(grid[0][0]) == false)				// Top-left
		new_grid[0][0] = tuple<double, bool>(0.5*(get<0>(grid[0][1]) + get<0>(grid[1][0])), 0);

	if (get<1>(grid[0][GRID_HEIGHT]) == false)	// Bottom-left
		new_grid[0][GRID_HEIGHT] = tuple<double, bool>(0.5*(get<0>(grid[0][GRID_HEIGHT - 1]) + get<0>(grid[1][GRID_HEIGHT])), 0);

	if (get<1>(grid[GRID_WIDTH][0]) == false)		// Top-right
		new_grid[GRID_WIDTH][0] = tuple<double, bool>(0.5*(get<0>(grid[GRID_WIDTH - 1][0]) + get<0>(grid[GRID_WIDTH][1])), 0);

	if (get<1>(grid[GRID_WIDTH][GRID_HEIGHT]) == false)	// Bottom-right
		new_grid[GRID_WIDTH][GRID_HEIGHT] = tuple<double, bool>(0.5*(get<0>(grid[GRID_WIDTH - 1][GRID_HEIGHT]) + get<0>(grid[GRID_WIDTH][GRID_HEIGHT - 1])), 0);


	// Next, update the edge pieces:
	// Top edge
	for (size_t i = 1; i < GRID_WIDTH - 1; ++i) {
		if (get<1>(grid[i][0]) == false)
			new_grid[i][0] = tuple<double, bool>((get<0>(grid[i - 1][0]) + get<0>(grid[i + 1][0]) + get<0>(grid[i][1])) / 3, 0);
	}
	// Bottom edge
	for (size_t i = 1; i < GRID_WIDTH - 1; ++i) {
		if (get<1>(grid[i][GRID_HEIGHT - 1]) == false)
			new_grid[i][GRID_HEIGHT - 1] = tuple<double, bool>((get<0>(grid[i - 1][GRID_HEIGHT - 1]) + get<0>(grid[i + 1][GRID_HEIGHT - 1]) + get<0>(grid[i][GRID_HEIGHT - 2])) / 3, 0);
	}
	// Left edge
	for (size_t j = 1; j < GRID_HEIGHT - 1; ++j) {
		if (get<1>(grid[0][j]) == false)
			new_grid[0][j] = tuple<double, bool>((get<0>(grid[0][j - 1]) + get<0>(grid[0][j + 1]) + get<0>(grid[1][j])) / 3, 0);
	}
	// Right edge
	for (size_t j = 1; j < GRID_HEIGHT - 1; ++j) {
		if (get<1>(grid[GRID_WIDTH - 1][j]) == false)
			new_grid[GRID_WIDTH - 1][j] = tuple<double, bool>((get<0>(grid[GRID_WIDTH - 1][j - 1]) + get<0>(grid[GRID_WIDTH - 1][j + 1]) + get<0>(grid[GRID_WIDTH - 2][j])) / 3, 0);
	}

	// Finally, update the interior cells
	for (size_t i = 1; i < GRID_WIDTH - 1; ++i) {
		for (size_t j = 1; j < GRID_HEIGHT - 1; ++j) {
			if (get<1>(grid[i][j]) == false)
				new_grid[i][j] = tuple<double, bool>((get<0>(grid[i][j - 1]) + get<0>(grid[i - 1][j]) + get<0>(grid[i + 1][j]) + get<0>(grid[i][j + 1])) / 4, 0);
		}
	}

	// grid = new_grid
	//copy(&new_grid[0][0], &new_grid[0][0] + GRID_WIDTH*GRID_HEIGHT, &grid[0][0]);
	for (size_t i = 0; i < GRID_WIDTH; ++i) {
		for (size_t j = 0; j < GRID_HEIGHT; ++j) {
			grid[i][j] = new_grid[i][j];
		}
	}
}

/**************************************************************************************************************************************************/
	
// Writes the current grid to a given output file so it can be plotted
void Board::writeBoard(string ofilename) {
	ofstream ofile;
	ofile.open(ofilename);
	for (int i = 0; i < GRID_WIDTH; ++i) {
		for (int j = 0; j < GRID_HEIGHT; ++j) {
			ofile << i << " " << j << " " << get<0>(grid[i][j]) << endl;
		}
		ofile << endl;
	}
	ofile.close();

	return;
}
