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
	int iterations;
	string shapes_file_name;

	cout << "Welcome to the E-Field Solver!" << endl << endl;
	cout << "How many iterations would you like to use? " << endl;
	cin >> iterations;

	// Create a new board, initialised according to the given shapes file
	Board board;
	cout << "Created a new board" << endl;	// Test
	board.textInitialise();
	cout << "Ok, I've initialised the board." << endl; // Test
	for (int i = 0; i < iterations; ++i) {
		board.jacobiUpdate();
	}
	cout << "I've run the simulation" << endl; // Test
	board.writeBoard("out.txt");
	cout << "I've written the board to out.txt" << endl; // Test

	exit(0);
}




/* Format of shapes file:
	origin_x origin_y width height potential */
