#include "stdafx.h"
#include "board.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <tuple>
#include <cstddef>

using namespace std;




int main()
{
	const size_t BOARD_WIDTH = 101;
	const size_t BOARD_HEIGHT = 101;
	const int ITERATIONS = 1500;
	string shapes_file_name;

	cout << "Welcome to the E-Field Solver!" << endl << endl;

	// Allow the user to enter any charged rectangles. Rectangles are stored in a vector.
	cout << "Input the name of a text file containing shape data: " << endl;
	cin >> shapes_file_name;
	cout << "Ok, I've read that file" << endl; // Test

	// Create a new board, initialised according to the given shapes file
	Board board;
	cout << "Created a new board" << endl;	// Test
	board.textInitialise(shapes_file_name);
	board.writeBoard("out.txt");

	return 0;
}


	/*

	// Initialise the board - a 2-dimensional array of tuples, with the first entry a double representing the potential at this point, and the second a boolean.
	// The boolean is 0 if this point is part of a charged shape, indicating that its potential is part of the boundary conditions and cannot be changed.
	tuple<double, bool> board[BOARD_WIDTH][BOARD_HEIGHT];
	
	// Set up the boundary conditions on the board according to the instructions provided in the shapes file
	ifstream shapes_file;
	shapes_file.open(shapes_file_name);
	string s;
	// For each rectangle
	while (getline(shapes_file, s)) 
	{
		// Break the line into its constituent ints, representing the properties of the rectangle
		vector<int> properties = split(s);
		vector<int>::const_iterator iter = properties.begin();
		int origin_x = *iter;
		++iter;
		int origin_y = *iter;
		++iter;
		int width = *iter;
		++iter;
		int height = *iter;
		++iter;
		int potential = *iter;

		// Enter the given potential at every point on the board that lies within the rectangle.
		// For each x-coord within the rectangle
		for (size_t i = origin_x; i < origin_x + width; ++i) {
			// For each y-coord within the rectangle
			for (size_t j = origin_y; j < origin_y + height; ++j) {
				// Set the potential of this cell.
				get<0>(board[i][j]) = potential;
				// Mark this cell as permanent
				get<1>(board[i][j]) = 1;
			}
		}
	}

	shapes_file.close();




	// Perform the relaxation algorithm on the board:

	// Make a copy of the board
	tuple<double, bool> new_board[BOARD_WIDTH][BOARD_HEIGHT];
	copy(&board[0][0], &board[0][0] + BOARD_WIDTH*BOARD_HEIGHT, &new_board[0][0]);

	// For each iteration:
	for (int t = 0; t < ITERATIONS; ++t) {

		// First update the corner pieces:

		if (get<1>(board[0][0]) == false)				// Top-left
			new_board[0][0] = tuple<double,bool>(0.5*(get<0>(board[0][1]) + get<0>(board[1][0])),0);

		if (get<1>(board[0][BOARD_HEIGHT]) == false)	// Bottom-left
			new_board[0][BOARD_HEIGHT] = tuple<double, bool>(0.5*(get<0>(board[0][BOARD_HEIGHT - 1]) + get<0>(board[1][BOARD_HEIGHT])), 0);

		if (get<1>(board[BOARD_WIDTH][0]) == false)		// Top-right
			new_board[BOARD_WIDTH][0] = tuple<double, bool>(0.5*(get<0>(board[BOARD_WIDTH - 1][0]) + get<0>(board[BOARD_WIDTH][1])), 0);

		if (get<1>(board[BOARD_WIDTH][BOARD_HEIGHT]) == false)	// Bottom-right
			new_board[BOARD_WIDTH][BOARD_HEIGHT] = tuple<double, bool>(0.5*(get<0>(board[BOARD_WIDTH - 1][BOARD_HEIGHT]) + get<0>(board[BOARD_WIDTH][BOARD_HEIGHT - 1])), 0);


		// Next, update the edge pieces:
		// Top edge
		for (size_t i = 1; i < BOARD_WIDTH - 1; ++i) {
			if (get<1>(board[i][0]) == false)
				new_board[i][0] = tuple<double, bool>((get<0>(board[i - 1][0]) + get<0>(board[i + 1][0]) + get<0>(board[i][1])) / 3, 0);
		}
		// Bottom edge
		for (size_t i = 1; i < BOARD_WIDTH - 1; ++i) {
			if (get<1>(board[i][BOARD_HEIGHT - 1]) == false)
				new_board[i][BOARD_HEIGHT - 1] = tuple<double, bool>((get<0>(board[i - 1][BOARD_HEIGHT - 1]) + get<0>(board[i + 1][BOARD_HEIGHT - 1]) + get<0>(board[i][BOARD_HEIGHT - 2])) / 3, 0);
		}
		// Left edge
		for (size_t j = 1; j < BOARD_HEIGHT - 1; ++j) {
			if (get<1>(board[0][j]) == false)
				new_board[0][j] = tuple<double, bool>((get<0>(board[0][j - 1]) + get<0>(board[0][j + 1]) + get<0>(board[1][j])) / 3, 0);
		}
		// Right edge
		for (size_t j = 1; j < BOARD_HEIGHT - 1; ++j) {
			if (get<1>(board[BOARD_WIDTH - 1][j]) == false)
				new_board[BOARD_WIDTH - 1][j] = tuple<double, bool>((get<0>(board[BOARD_WIDTH - 1][j - 1]) + get<0>(board[BOARD_WIDTH - 1][j + 1]) + get<0>(board[BOARD_WIDTH - 2][j])) / 3, 0);
		}

		// Finally, update the interior cells
		for (size_t i = 1; i < BOARD_WIDTH - 1; ++i) {
			for (size_t j = 1; j < BOARD_HEIGHT - 1; ++j) {
				if (get<1>(board[i][j]) == false)
					new_board[i][j] = tuple<double, bool>((get<0>(board[i][j - 1]) + get<0>(board[i - 1][j]) + get<0>(board[i + 1][j]) + get<0>(board[i][j + 1])) / 4, 0);
			}
		}

		// board = new_board
		copy(&new_board[0][0], &new_board[0][0] + BOARD_WIDTH*BOARD_HEIGHT, &board[0][0]);
	}




	string ofile_name;
	cout << "Enter the name of the output file:" << endl;
	cin >> ofile_name;
	ofstream ofile;
	ofile.open(ofile_name);
	for (int i = 0; i < BOARD_WIDTH; ++i) {
		for (int j = 0; j < BOARD_HEIGHT; ++j) {
			ofile << i << " " << j << " " << get<0>(board[i][j]) << endl;
		}
		ofile << endl;
	}
	ofile.close();

	return 0;
}


*/


/* Format of shapes file:
	origin_x origin_y width height potential */
