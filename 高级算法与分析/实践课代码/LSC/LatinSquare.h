////////////////////////////////
/// usage : 1.	SDK for graph coloring solver.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef CN_HUST_SZX_NPBENCHMARK_LATIN_SQUARE_H
#define CN_HUST_SZX_NPBENCHMARK_LATIN_SQUARE_H


#include <array>
#include <vector>
#include <functional>
// #include "utils.h"
#include <fstream>

struct Assignment {
	int row;
	int col;
	int num;
};

namespace szx {

using Num = int;

struct LatinSquare {
	Num n;
	std::vector<Assignment> fixedNums; // fixed numbers.
};

using Table = std::vector<std::vector<Num>>; // a 2D array of numbers.


void solveLatinSquare(std::ostream& outputStream, LatinSquare& input, std::function<long long()> restMilliSec, int seed);

}


#endif // CN_HUST_SZX_NPBENCHMARK_LATIN_SQUARE_H
