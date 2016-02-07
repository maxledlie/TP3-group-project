#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Functions for string manipulation
bool space(char c)
{
	return isspace(c);
}

bool not_space(char c)
{
	return !isspace(c);
}

// Splits a line of integers into a vector of integers
vector<int> split(const string& str)
{
	typedef string::const_iterator iter;
	vector<int> ret;

	iter i = str.begin();
	while (i != str.end()) {
		// ignore leading blanks
		i = find_if(i, str.end(), not_space);

		// find end of next number
		iter j = find_if(i, str.end(), space);

		// copy the characters in [i, j)
		if (i != str.end()){
			ret.push_back(stoi(string(i, j)));
		}
		i = j;
	}
	return ret;
}
