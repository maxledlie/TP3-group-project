#include "board.h"
#include "bitmap_image.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>


using namespace std;


int main()
{
	string bmp_file_name;
	string out_file_name;
	unsigned int iterations;

	cout << "Welcome to the E-Field Solver!" << endl << endl;
	cout << "Enter the name of the bitmap file to use as input: " << endl;
	cin >> bmp_file_name;
	cout << "Enter the name of the file to output the data to: " << endl;
	cin >> out_file_name;
	cout << "Enter the number of iterations to use: " << endl;
	cin >> iterations;

	// Create a new board initialised according to the provided .bmp file
	Board board;
	board.bmpInitialise(bmp_file_name);
	cout << "Initialised the board" << endl; // Test

	for (int i = 0; i < iterations; ++i)
		board.jacobiUpdate();

	board.writeBoard(out_file_name);
	return 0;
}
