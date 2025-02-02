#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <cstring>

#include "LatinSquare.h"


using namespace std;
using namespace std::chrono;
using namespace szx;


void loadInput(istream& is, LatinSquare& lsc) {
	is >> lsc.n;
	lsc.fixedNums.reserve(lsc.n * lsc.n);
	for (Assignment a; is >> a.row >> a.col >> a.num; lsc.fixedNums.push_back(a)) {}
}

void saveOutput(ostream& os, Table& assignments) {
	for (auto i = assignments.begin(); i != assignments.end(); ++i) {
		for (auto j = i->begin(); j != i->end(); ++j) { os << *j << '\t'; }
		os << endl;
	}
}

void test(istream& inputStream, ostream& outputStream, long long secTimeout, int randSeed) {
	cerr << "load input." << endl;
	LatinSquare lsc;
	loadInput(inputStream, lsc);

	cerr << "solve." << endl;
	steady_clock::time_point endTime = steady_clock::now() + seconds(secTimeout);
	solveLatinSquare(outputStream, lsc, [&]() { return duration_cast<milliseconds>(endTime - steady_clock::now()).count(); }, randSeed);
}
void test(istream& inputStream, ostream& outputStream, long long secTimeout) {
	return test(inputStream, outputStream, secTimeout, static_cast<int>(time(nullptr) + clock()));
}


int main(int argc, char* argv[]) {
	cerr << "load environment." << endl;
	if (argc > 2) {
		long long secTimeout = atoll(argv[1]);
		int randSeed = atoi(argv[2]);
		test(cin, cout, secTimeout, randSeed);
	} else {
		//string filename = "./Instance/LSC.n70f3920.00.txt";
		//for (int i = 0; i < 100; ++i) {
		//	int h = i / 10, l = i % 10;
		//	filename[24] = h + '0';
		//	filename[25] = l + '0';
		//	cerr << "Current file: " << filename << endl;
		//	ifstream ifs(filename);
		//	ofstream ofs("./Instance/solution.txt");
		//	test(ifs, ofs, 5); // for self-test.
		//	cerr << "*****************" << endl;
		//}

		for (int i = 0; i < 5; ++i) {
			ifstream ifs("./instance/4.txt");
			ofstream ofs("./instance/solution.txt");
			test(ifs, ofs, 1500); // for self-test
		}
	}
	return 0;
}
