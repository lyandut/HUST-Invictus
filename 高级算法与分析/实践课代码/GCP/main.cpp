#include <iostream>
#include <string>
#include <chrono>
#include <fstream>

#include "GraphColoring.h"


using namespace std;
using namespace std::chrono;
using namespace szx;


void loadInput(istream& is, GraphColoring& gc) {
	is >> gc.nodeNum >> gc.edgeNum >> gc.colorNum;
	gc.edges.resize(gc.edgeNum);
	gc.NeighborTable.resize(gc.nodeNum);
	for (auto edge = gc.edges.begin(); edge != gc.edges.end(); ++edge) {
		int i, j;
		is >> i >> j;
		(*edge)[0] = i;
		(*edge)[1] = j;
	
		gc.NeighborTable[i].push_back(j);
		gc.NeighborTable[j].push_back(i);
	}
}

void saveOutput(ostream& os, NodeColors& nodeColors) {
	for (auto color = nodeColors.begin(); color != nodeColors.end(); ++color) { os << *color << endl; }
}

void test(istream& inputStream, ostream& outputStream, long long secTimeout, int randSeed) {
	cerr << "load input." << endl;
	GraphColoring gc;
	loadInput(inputStream, gc);

	cerr << "solve." << endl;
	steady_clock::time_point start = steady_clock::now();
	steady_clock::time_point endTime = steady_clock::now() + seconds(secTimeout);
	NodeColors nodeColors(gc.nodeNum);
	solveGraphColoring(nodeColors, gc, [&]() { return duration_cast<milliseconds>(endTime - steady_clock::now()).count(); }, randSeed);
	steady_clock::time_point end = steady_clock::now();

	cerr << "save output." << endl;
	saveOutput(outputStream, nodeColors);

	int f = 0;
	for (auto edge : gc.edges) 
		if (nodeColors[edge[0]] == nodeColors[edge[1]])
			++ f;

	uint64_t duration = duration_cast<milliseconds>(end - start).count();
	cerr << "Run time: " << duration << "ms" << endl;
	cerr << "conflicted edge nums: " << f << endl;
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
	}
	else {
		ifstream ifs("./instance/500.1.txt");
		ofstream ofs("./instance/solution.txt");
		test(ifs, ofs, 100000); // for self-test.
	}
	return 0;
}
