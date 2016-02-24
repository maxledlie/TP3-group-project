#include "board.h"
#include "bitmap_image.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
#include <time.h>

using namespace std;

// Returns the part of the string after the last '.'
string file_extension(string filename) {
	string out = "";
	string::size_type back = 0;
	while (filename[filename.size() - back] != '.') {
		out += filename[filename.size() - back];
		back += 1;
		if (back >= filename.size())
			return "";
	}
	reverse(out.begin(), out.end());
	out.pop_back();	// Remove annoying whitespace that's turned up for whatever reason.
	return out;
}


int main(int argc, char *argv[])
{
	string bmp_file_name = argv[1];
	string out_file_name = argv[2];
	string vector_file_name = argv[3];
	string simulation;
	unsigned int max_iterations;
	unsigned int iterations;
	double precision;


	// Catch invalid inputs
	cout << "Welcome to the E-Field Solver!" << endl << endl;
	if (file_extension(bmp_file_name).compare("bmp") != 0) {
		cout << "The input file must be a .bmp file!" << endl;
		return 1;
	}
	if (file_extension(out_file_name).compare("txt") != 0) {
		cout << "The output file must be a .txt file!" << endl;
		return 1;
	}
	if (file_extension(vector_file_name).compare("txt") != 0) {
		cout << "The output file must be a .txt file!" << endl;
		return 1;
	}

	cout << "Enter the precision required: " << endl;
	cin >> precision;

	cout << "Enter the maximum number of iterations: " << endl;
	cin >> max_iterations;

	// Create a new board initialised according to the provided .bmp file
	Board board;
	board.bmpInitialise(bmp_file_name);
	cout << "Initialised the board" << endl << endl; 

	cout << "Which method would you like to use to run the simulation?" << endl;
	cout << "Options: 'Jacobi', 'Gauss', 'SOR'" << endl;
	cout << ">> ";
	cin >> simulation;

	if (simulation == "Jacobi" || simulation == "jacobi" || simulation == "J" || simulation == "j") {
		clock_t start_time = clock();
		board.jacobiUpdate();
		iterations = 1;
		for (int i = 1; i < max_iterations && board.converged(precision) == false; ++i) {
			board.jacobiUpdate();
			iterations++;
		}
		cout << double(clock() - start_time) / (double)CLOCKS_PER_SEC << " seconds." << endl;
	}

	else if (simulation == "Gauss" || simulation == "gauss" || simulation == "G" || simulation == "g") {
		clock_t start_time = clock();
		board.gaussUpdate();
		iterations = 1;
		for (int i = 1; i < max_iterations && board.converged(precision) == false; ++i) {
			board.gaussUpdate();
			iterations++;
		}
		cout << double(clock() - start_time) / (double)CLOCKS_PER_SEC << " seconds." << endl;
	}

	else if (simulation == "SOR" || simulation == "sor" || simulation == "S" || simulation == "s") {
		clock_t start_time = clock();
		board.sorUpdate();
		iterations = 1;
		for (int i = 1; i < max_iterations && board.converged(precision) == false; ++i) {
			board.sorUpdate();
			iterations++;
		}
		cout << double(clock() - start_time) / (double)CLOCKS_PER_SEC << " seconds." << endl;
	}

	else
		cout << "Not a valid simulation method!" << endl;

	cout << "The number of iterations was " << iterations << endl;

	board.writeBoard(out_file_name, vector_file_name);
	return 0;
}
