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
	string simulation;
	unsigned int max_iterations;

	cout << "Welcome to the E-Field Solver!" << endl << endl;
	cout << "Enter the name of the bitmap file to use as input: " << endl;
	cin >> bmp_file_name;
	cout << "Enter the name of the file to output the data to: " << endl;
	cin >> out_file_name;
	cout << "Enter the maximum number of iterations to use: " << endl;
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
		for (int i = 0; i < max_iterations && !board.converged(); ++i)
			board.jacobiUpdate();
	}

	else if (simulation == "Gauss" || simulation == "gauss" || simulation == "G" || simulation == "g") {
		for (int i = 0; i < max_iterations && !board.converged(); ++i)
			board.gaussUpdate();
	}

	else if (simulation == "SOR" || simulation == "sor" || simulation == "S" || simulation == "s") {
		for (int i = 0; i < max_iterations && !board.converged(); ++i)
			board.sorUpdate();
	}

	else
		cout << "Not a valid simulation method!" << endl;

	board.writeBoard(out_file_name);
	return 0;
}

