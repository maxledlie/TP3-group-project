#include "bitmap_image.hpp"
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
	vector<vector<tuple<double, bool>>> grid;

	void bmpInitialise(string bmp_file_name);
	void jacobiUpdate();
	void writeBoard(string ofile_name);



};

// Member function definitions:

/*****************************************************************************************************************************************/
// Default constructor:
// Initialise the grid based on the provided .bmp image. (RED = +5V, GREEN = 0V, BLUE = -5V)
void Board::bmpInitialise(string bmp_file_name) {

	bitmap_image image(bmp_file_name);

	if (!image){
		printf("Error: Failed to open input.bmp\n");
	}

	unsigned char red;
	unsigned char green;
	unsigned char blue;

	int grid_width = image.width();
	int grid_height = image.height();

	for (int i = 0; i < grid_width; ++i) {
		grid.push_back(vector<tuple<double, bool>>());
	}
	for (int i = 0; i < grid_width; ++i) {
		for (int j = 0; j < grid_height; ++j) {
			grid[i].push_back(make_tuple(0, false));
		}
	}

	for (int x = 0; x < grid_width; ++x) {
		for (int y = 0; y < grid_height; ++y) {
			image.get_pixel(x, y, red, green, blue);
			if (red == 255 && green == 0 && blue == 0)
				grid[x][y] = make_tuple(5, true);
			else if (red == 0 && green == 255 && blue == 0)
				grid[x][y] = make_tuple(0, true);
			else if (red == 0 && green == 0 && blue == 255)
				grid[x][y] = make_tuple(-5, true);
		}
	}

	return;
}




// Performs one iteration of the Jacobi method of relaxation. SLOW! ***************************************************************************************
void Board::jacobiUpdate() {

	int grid_width = grid.size();
	int grid_height = grid[0].size();

	// Make a copy of the grid
	vector<vector<tuple<double, bool>>> new_grid = grid;
	for (int i = 0; i < grid_width; ++i) {
		for (int j = 0; j < grid_height; ++j) {
			new_grid[i][j] = grid[i][j];
		}
	}
		
	// First update the corner pieces:

	if (get<1>(grid[0][0]) == false)				// Top-left
		new_grid[0][0] = tuple<double, bool>(0.5*(get<0>(grid[0][1]) + get<0>(grid[1][0])), false);

	if (get<1>(grid[0][grid_height-1]) == false)	// Bottom-left
		new_grid[0][grid_height-1] = tuple<double, bool>(0.5*(get<0>(grid[0][grid_height - 2]) + get<0>(grid[1][grid_height - 1])), false);

	if (get<1>(grid[grid_width - 1][0]) == false)		// Top-right
		new_grid[grid_width - 1][0] = tuple<double, bool>(0.5*(get<0>(grid[grid_width - 2][0]) + get<0>(grid[grid_width - 1][1])), false);

	if (get<1>(grid[grid_width - 1][grid_height - 1]) == false)	// Bottom-right
		new_grid[grid_width - 1][grid_height - 1] = tuple<double, bool>(0.5*(get<0>(grid[grid_width - 2][grid_height - 1]) + get<0>(grid[grid_width - 1][grid_height - 2])), false);


	// Next, update the edge pieces:
	// Top edge
	for (int i = 1; i < grid_width - 1; ++i) {
		if (get<1>(grid[i][0]) == false)
			new_grid[i][0] = tuple<double, bool>((get<0>(grid[i - 1][0]) + get<0>(grid[i + 1][0]) + get<0>(grid[i][1])) / 3, false);
	}
	// Bottom edge
	for (int i = 1; i < grid_width - 1; ++i) {
		if (get<1>(grid[i][grid_height - 1]) == false)
			new_grid[i][grid_height - 1] = tuple<double, bool>((get<0>(grid[i - 1][grid_height - 1]) + get<0>(grid[i + 1][grid_height - 1]) + get<0>(grid[i][grid_height - 2])) / 3, false);
	}
	// Left edge
	for (int j = 1; j < grid_height - 1; ++j) {
		if (get<1>(grid[0][j]) == false)
			new_grid[0][j] = tuple<double, bool>((get<0>(grid[0][j - 1]) + get<0>(grid[0][j + 1]) + get<0>(grid[1][j])) / 3, false);
	}
	// Right edge
	for (int j = 1; j < grid_height - 1; ++j) {
		if (get<1>(grid[grid_width - 1][j]) == false)
			new_grid[grid_width - 1][j] = tuple<double, bool>((get<0>(grid[grid_width - 1][j - 1]) + get<0>(grid[grid_width - 1][j + 1]) + get<0>(grid[grid_width - 2][j])) / 3, false);
	}

	// Finally, update the interior cells
	for (int i = 1; i < grid_width - 1; ++i) {
		for (int j = 1; j < grid_height - 1; ++j) {
			if (get<1>(grid[i][j]) == false)
				new_grid[i][j] = tuple<double, bool>((get<0>(grid[i][j - 1]) + get<0>(grid[i - 1][j]) + get<0>(grid[i + 1][j]) + get<0>(grid[i][j + 1])) / 4, false);
		}
	}

	// grid = new_grid
	//copy(&new_grid[0][0], &new_grid[0][0] + grid_width*grid_height, &grid[0][0]);
	for (int i = 0; i < grid_width; ++i) {
		for (int j = 0; j < grid_height; ++j) {
			grid[i][j] = new_grid[i][j];
		}
	}
}

	
// Writes the current grid to a given output file so it can be plotted *************************************************************************************
void Board::writeBoard(string ofilename) {

	int grid_width = grid.size();
	int grid_height = grid[0].size();
 	ofstream ofile;
	cout << "Writing output to " << ofilename << endl;
	ofile.open(ofilename);
	for (int i = 0; i < grid_width; ++i) {
		for (int j = 0; j < grid_height; ++j) {
			ofile << i << " " << j << " " << get<0>(grid[i][j]) << endl;
		}
		ofile << endl;
	}
	ofile.close();

	return;
}
