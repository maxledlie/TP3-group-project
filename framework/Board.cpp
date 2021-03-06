#include "bitmap_image.hpp"
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
	vector<vector<tuple<double, bool>>> new_grid;

	void bmpInitialise(string bmp_file_name);
	bool converged(double);
	void jacobiUpdate();
	void gaussUpdate();
	void sorUpdate();
  void writeBoard(string ofile_name, string vfilename);



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
		new_grid.push_back(vector<tuple<double, bool>>());
	}
	for (int i = 0; i < grid_width; ++i) {
		for (int j = 0; j < grid_height; ++j) {
			new_grid[i].push_back(make_tuple(0, false));
		}
	}

	for (int x = 0; x < grid_width; ++x) {
		for (int y = 0; y < grid_height; ++y) {
			image.get_pixel(x, y, red, green, blue);
			if (red == 255 && green == 0 && blue == 0)
				new_grid[x][y] = make_tuple(5, true);
			else if (red == 0 && green == 255 && blue == 0)
				new_grid[x][y] = make_tuple(0, true);
			else if (red == 0 && green == 0 && blue == 255)
				new_grid[x][y] = make_tuple(-5, true);
		}
	}

	return;
}



// Check if the simulation has converged - that is, if 'new_grid' is sufficiently similar to 'grid' ***************************************************
bool Board::converged(double precision) {


	int grid_width = new_grid.size();
	int grid_height = new_grid[0].size();
	int errorcount = 0;
	double error = 0;

	for (int i = 0; i < grid_width; ++i)	  {
		for (int j = 0; j < grid_height; ++j) {
			error = abs(get<0>(grid[i][j]) - get<0>(new_grid[i][j]));
			if (error <= precision)
			{
				errorcount += 1; //adds point to total points that meet precision threshold
			}
		}
	}

	bool converged = false;
	//if all points are precise enough, then do not repeat
	if (errorcount == grid_width*grid_height){
		converged = true;
	}

	return converged;

}


// Performs one iteration of the Jacobi method of relaxation. SLOW! ***************************************************************************************
// After this method is run, new_grid holds values that are one iteration further along than 'grid'
void Board::jacobiUpdate() {

	grid = new_grid;

	int grid_width = new_grid.size();
	int grid_height = new_grid[0].size();

		
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
}


// Performs one iteration of the Gaussian simulation ******************************************************************************************************
// Author: Max S. Modified to work with converged().
void Board::gaussUpdate() {

	int grid_width = new_grid.size();
	int grid_height = new_grid[0].size();

	grid = new_grid;

	// First update the corner pieces:

	if (get<1>(new_grid[0][0]) == false)				// Top-left
		new_grid[0][0] = tuple<double, bool>(0.5*(get<0>(new_grid[0][1]) + get<0>(new_grid[1][0])), false);

	if (get<1>(new_grid[0][grid_height - 1]) == false)	// Bottom-left
		new_grid[0][grid_height - 1] = tuple<double, bool>(0.5*(get<0>(new_grid[0][grid_height - 2]) + get<0>(new_grid[1][grid_height - 1])), false);

	if (get<1>(new_grid[grid_width - 1][0]) == false)		// Top-right
		new_grid[grid_width - 1][0] = tuple<double, bool>(0.5*(get<0>(new_grid[grid_width - 2][0]) + get<0>(new_grid[grid_width - 1][1])), false);

	if (get<1>(new_grid[grid_width - 1][grid_height - 1]) == false)	// Bottom-right
		new_grid[grid_width - 1][grid_height - 1] = tuple<double, bool>(0.5*(get<0>(new_grid[grid_width - 2][grid_height - 1]) + get<0>(new_grid[grid_width - 1][grid_height - 2])), false);

	// Next, update the edge pieces:
	// Top edge
	for (int i = 1; i < grid_width - 1; ++i) {
		if (get<1>(new_grid[i][0]) == false)
			new_grid[i][0] = tuple<double, bool>((get<0>(new_grid[i - 1][0]) + get<0>(new_grid[i + 1][0]) + get<0>(new_grid[i][1])) / 3, false);
	}
	// Bottom edge
	for (int i = 1; i < grid_width - 1; ++i) {
		if (get<1>(new_grid[i][grid_height - 1]) == false)
			new_grid[i][grid_height - 1] = tuple<double, bool>((get<0>(new_grid[i - 1][grid_height - 1]) + get<0>(new_grid[i + 1][grid_height - 1]) + get<0>(new_grid[i][grid_height - 2])) / 3, false);
	}
	// Left edge
	for (int j = 1; j < grid_height - 1; ++j) {
		if (get<1>(new_grid[0][j]) == false)
			new_grid[0][j] = tuple<double, bool>((get<0>(new_grid[0][j - 1]) + get<0>(new_grid[0][j + 1]) + get<0>(new_grid[1][j])) / 3, false);

	}
	// Right edge
	for (int j = 1; j < grid_height - 1; ++j) {
		if (get<1>(new_grid[grid_width - 1][j]) == false)
			new_grid[grid_width - 1][j] = tuple<double, bool>((get<0>(new_grid[grid_width - 1][j - 1]) + get<0>(new_grid[grid_width - 1][j + 1]) + get<0>(new_grid[grid_width - 2][j])) / 3, false);
	}

	// Finally, update the interior cells
	for (int i = 1; i < grid_width - 1; ++i) {
		for (int j = 1; j < grid_height - 1; ++j) {
			if (get<1>(new_grid[i][j]) == false)
				new_grid[i][j] = tuple<double, bool>((get<0>(new_grid[i][j - 1]) + get<0>(new_grid[i - 1][j]) + get<0>(new_grid[i + 1][j]) + get<0>(new_grid[i][j + 1])) / 4, false);
		}
	}
}


// TODO: Performs one iteration of the SOR simulation with relaxation constant 'omega' ***************************************************************************
// Author: Wenbo. Modified to work with converged().
void Board::sorUpdate() {

	int grid_width = new_grid.size();
	int grid_height = new_grid[0].size();
	double omega = 2 / (1 + 3.14159 / grid_width);

	grid = new_grid;

	// First update the corner pieces:

	if (get<1>(grid[0][0]) == false)				// Top-left
		new_grid[0][0] = tuple<double, bool>(0.5*(get<0>(grid[0][1]) + get<0>(grid[1][0])), false);

	if (get<1>(grid[0][grid_height - 1]) == false)	// Bottom-left
		new_grid[0][grid_height - 1] = tuple<double, bool>(0.5*(get<0>(grid[0][grid_height - 2]) + get<0>(grid[1][grid_height - 1])), false);

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
				new_grid[i][j] = tuple<double, bool>((1 - omega)*get<0>(grid[i][j]) + omega*(get<0>(new_grid[i][j - 1]) + get<0>(new_grid[i - 1][j]) + get<0>(grid[i + 1][j]) + get<0>(grid[i][j + 1])) / 4, false);
		}
	}
}

	
// Writes the current grid to a given output file so it can be plotted *************************************************************************************
void Board::writeBoard(string ofilename, string vfilename) {

	int grid_width = new_grid.size();
	int grid_height = new_grid[0].size();
 	ofstream ofile;
	cout << "Writing output to " << ofilename << endl;
	ofile.open(ofilename);
	for (int i = 0; i < grid_width; ++i) {
		for (int j = 0; j < grid_height; ++j) {
			ofile << i << " " << j << " " << get<0>(new_grid[i][j]) << endl;
		}
		ofile << endl;
	}
	ofile.close();
	
	ofstream vfile;
	  cout << "Writing vector file to " << vfilename <<endl;
	vfile.open(vfilename);
	for (int i = 1; i < grid_width-1; ++i) {
		for (int j = 1; j < grid_height-1; ++j) {
		  vfile << i << " " << j << " " << ((get<0>(new_grid[i-1][j])-get<0>(new_grid[i+1][j]))/(2))<< " " << ((get<0>(new_grid[i][j-1])-get<0>(new_grid[i][j+1]))/(2)) << endl;
		}
		vfile << endl;
	}
	vfile.close();
	
	return;
}

	
