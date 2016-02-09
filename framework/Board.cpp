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
	void jacobiUpdate();
	void writeBoard(string ofile_name);
	void textInitialise();

	vector<vector<tuple<double,bool>>> grid;

};

// Member function definitions:

/*****************************************************************************************************************************************/
// Default constructor:
// Initialise the grid to be an empty grid of the given dimensions.
Board::Board(void) {
	for (size_t i = 0; i < GRID_WIDTH; ++i) {
		grid.push_back(vector<tuple<double, bool>>());
	}
	for (size_t i = 0; i < GRID_WIDTH; ++i) {
		for (size_t j = 0; j < GRID_HEIGHT; ++j) {
			grid[i].push_back(make_tuple(0, false));
		}
	}
};


// Initialises a board from a text file
void Board::textInitialise() {

	ifstream shapes_file;
	shapes_file.open("shapes.txt");
	string s;
	// For each rectangle
	while (getline(shapes_file, s)){
		cout << "Reading a line... " << endl; // Test

		// Break the line into its constituent ints, representing the properties of the rectangle
		vector<int> properties = split(s);
		vector<int>::const_iterator iter = properties.begin();
		size_t origin_x = *iter;
		cout << "The x-origin is " << origin_x << endl; // Test
		++iter;
		size_t origin_y = *iter;
		cout << "The y-origin is " << origin_y << endl; // Test
		++iter;
		size_t width = *iter;
		cout << "The width is " << width << endl; // Test
		++iter;
		size_t height = *iter;
		cout << "The height is " << height << endl; // Test
		++iter;
		int potential = *iter;

		// Check if the rectangle will fit on the board
		if (origin_x < GRID_WIDTH && origin_x + width <= GRID_WIDTH && origin_y < GRID_HEIGHT && origin_y + height <= GRID_HEIGHT) {
			// Enter the given potential at every point on the grid that lies within the rectangle.
			// For each x-coord within the rectangle
			for (size_t i = origin_x; i < origin_x + width; ++i) {
				// For each y-coord within the rectangle
				for (size_t j = origin_y; j < origin_y + height; ++j) {
					// Set the potential of this cell and mark it as permanent.
					grid[i][j] = make_tuple(potential,true);

				}
			}
		}
	}
	shapes_file.close();
}


// Performs one iteration of the Jacobi method of relaxation. SLOW! ***************************************************************************************
void Board::jacobiUpdate() {

	// Make a copy of the grid
	vector<vector<tuple<double, bool>>> new_grid = grid;;
	//copy(&grid[0][0], &grid[0][0] + GRID_WIDTH*GRID_HEIGHT, &new_grid[0][0]);
	for (size_t i = 0; i < GRID_WIDTH; ++i) {
		for (size_t j = 0; j < GRID_HEIGHT; ++j) {
			new_grid[i][j] = grid[i][j];
		}
	}
		
	// First update the corner pieces:

	if (get<1>(grid[0][0]) == false)				// Top-left
		new_grid[0][0] = tuple<double, bool>(0.5*(get<0>(grid[0][1]) + get<0>(grid[1][0])), false);

	if (get<1>(grid[0][GRID_HEIGHT-1]) == false)	// Bottom-left
		new_grid[0][GRID_HEIGHT-1] = tuple<double, bool>(0.5*(get<0>(grid[0][GRID_HEIGHT - 2]) + get<0>(grid[1][GRID_HEIGHT - 1])), false);

	if (get<1>(grid[GRID_WIDTH - 1][0]) == false)		// Top-right
		new_grid[GRID_WIDTH - 1][0] = tuple<double, bool>(0.5*(get<0>(grid[GRID_WIDTH - 2][0]) + get<0>(grid[GRID_WIDTH - 1][1])), false);

	if (get<1>(grid[GRID_WIDTH - 1][GRID_HEIGHT - 1]) == false)	// Bottom-right
		new_grid[GRID_WIDTH - 1][GRID_HEIGHT - 1] = tuple<double, bool>(0.5*(get<0>(grid[GRID_WIDTH - 2][GRID_HEIGHT - 1]) + get<0>(grid[GRID_WIDTH - 1][GRID_HEIGHT - 2])), false);


	// Next, update the edge pieces:
	// Top edge
	for (size_t i = 1; i < GRID_WIDTH - 1; ++i) {
		if (get<1>(grid[i][0]) == false)
			new_grid[i][0] = tuple<double, bool>((get<0>(grid[i - 1][0]) + get<0>(grid[i + 1][0]) + get<0>(grid[i][1])) / 3, false);
	}
	// Bottom edge
	for (size_t i = 1; i < GRID_WIDTH - 1; ++i) {
		if (get<1>(grid[i][GRID_HEIGHT - 1]) == false)
			new_grid[i][GRID_HEIGHT - 1] = tuple<double, bool>((get<0>(grid[i - 1][GRID_HEIGHT - 1]) + get<0>(grid[i + 1][GRID_HEIGHT - 1]) + get<0>(grid[i][GRID_HEIGHT - 2])) / 3, false);
	}
	// Left edge
	for (size_t j = 1; j < GRID_HEIGHT - 1; ++j) {
		if (get<1>(grid[0][j]) == false)
			new_grid[0][j] = tuple<double, bool>((get<0>(grid[0][j - 1]) + get<0>(grid[0][j + 1]) + get<0>(grid[1][j])) / 3, false);
	}
	// Right edge
	for (size_t j = 1; j < GRID_HEIGHT - 1; ++j) {
		if (get<1>(grid[GRID_WIDTH - 1][j]) == false)
			new_grid[GRID_WIDTH - 1][j] = tuple<double, bool>((get<0>(grid[GRID_WIDTH - 1][j - 1]) + get<0>(grid[GRID_WIDTH - 1][j + 1]) + get<0>(grid[GRID_WIDTH - 2][j])) / 3, false);
	}

	// Finally, update the interior cells
	for (size_t i = 1; i < GRID_WIDTH - 1; ++i) {
		for (size_t j = 1; j < GRID_HEIGHT - 1; ++j) {
			if (get<1>(grid[i][j]) == false)
				new_grid[i][j] = tuple<double, bool>((get<0>(grid[i][j - 1]) + get<0>(grid[i - 1][j]) + get<0>(grid[i + 1][j]) + get<0>(grid[i][j + 1])) / 4, false);
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
	for (size_t i = 0; i < GRID_WIDTH; ++i) {
		for (size_t j = 0; j < GRID_HEIGHT; ++j) {
			ofile << i << " " << j << " " << get<0>(grid[i][j]) << endl;
		}
		ofile << endl;
	}
	ofile.close();

	return;
}
