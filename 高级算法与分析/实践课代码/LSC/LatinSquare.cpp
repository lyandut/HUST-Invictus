#include "LatinSquare.h"

#include <random>
#include <iostream>
#include <chrono>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <random>
#include <functional>
#include <fstream>

using namespace std;
using namespace std::chrono;

class BidirectionIndex {
private:
	int mapsize = 0;
	std::vector<int> map;
	std::vector<int> data;

public:
	BidirectionIndex() = default;
	BidirectionIndex(const BidirectionIndex& d) : mapsize(d.mapsize), map(d.map), data(d.data) {};
	BidirectionIndex(int n) : mapsize(n), map(n, -1) {};

	void init(int n);
	void insert(int d);
	void erase(int d);
	uint32_t size();
	bool find(int d);
	void clear();

	int operator[](int pos)	const { return data[pos]; };
};

void BidirectionIndex::init(int n) {
	map.assign(n + 1, -1);
}

void BidirectionIndex::insert(int d) {
	if (map[d] == -1) {
		data.push_back(d);
		map[d] = data.size() - 1;
	}
}

void BidirectionIndex::erase(int d) {
	if (map[d] == -1)
		return;

	int pos = map[d];
	data[pos] = data.back();
	map[data[pos]] = pos;
	data.pop_back();
	map[d] = -1;
}

uint32_t BidirectionIndex::size() {
	return data.size();
}

bool BidirectionIndex::find(int d) {
	return map[d] != -1;
}

void BidirectionIndex::clear() {
	map.assign(mapsize + 1, -1);
	data.clear();
}

class Work {
private:
	const int RT0 = 10, MAXRT = 15, MAXACCU = 1000;

	int VNum, CNum; // 节点数量和颜色数量
	std::vector<BidirectionIndex> ColorDomain; // 每个节点的颜色域
	std::vector<BidirectionIndex> NeighborTable; // 每个节点的邻居
	std::vector<BidirectionIndex> RowNeighborTable, ColNeighborTable; // 节点的行邻居和列邻居
	std::vector<bool> FixNodes; // 固定节点集或删除节点集
	std::vector<BidirectionIndex> RowColorDomain; // 行颜色域

	// 解结构
	std::vector<int> Sol, BestSol;
	int conflict = 0;

	// 禁忌搜索
	int ConflictNodeLen = 0;
	std::vector<int> ConflictNodes; // 冲突节点
	std::vector<int> ConflictNodePos; 
	std::vector<std::vector<int>> AdjColorTable; // 邻居颜色表
	std::vector<std::vector<int>> TabuTable; // 禁忌表
	std::vector<std::vector<bool>> ColorDomainTable; // 颜色域表	

	int besthistoryf; // 历史最好的f
	std::vector<int> EqualNontabuDeltaU, EqualNontabuDeltaV; // 最好非禁忌动作节点U和V
	std::vector<int> EqualTabuDeltaU, EqualTabuDeltaV; // 最好禁忌动作节点U和V
	int bestu, bestv, delta; // 最好动作

	// 随机数
	std::mt19937 pseudoRandNumGen;
	void initRand(int seed) { pseudoRandNumGen = std::mt19937(seed); }
	int fastRand(int lb, int ub) { return (pseudoRandNumGen() % (ub - lb)) + lb; }
	int fastRand(int ub) { return pseudoRandNumGen() % ub; }
	int rand(int lb, int ub) { return std::uniform_int_distribution<int>(lb, ub - 1)(pseudoRandNumGen); }
	int rand(int ub) { return std::uniform_int_distribution<int>(0, ub - 1)(pseudoRandNumGen); }


	void InitSol();
	int CheckRule(int Node);
	void FindMove(int iter);
	void MakeMove(int iter);
public:
	Work() = default;
	Work(int vnum, int cnum, const std::vector<Assignment>& fixNode, int seed);
	
	int solve(std::function<long long()> restMilliSec);
	void savesol(std::ostream& os);
	void check();
};

Work::Work(int vnum, int cnum, const vector<Assignment>& fixNode, int seed) :
	VNum(vnum),
	CNum(cnum),
	Sol(vnum),
	ColorDomain(vnum, BidirectionIndex(cnum)),
	RowColorDomain(cnum, BidirectionIndex(cnum)),
	NeighborTable(vnum, BidirectionIndex(vnum)),
	RowNeighborTable(vnum, BidirectionIndex(vnum)),
	ColNeighborTable(vnum, BidirectionIndex(vnum)),
	FixNodes(vnum, false),
	ConflictNodes(vnum),
	ConflictNodePos(vnum, -1),
	AdjColorTable(vnum, vector<int>(vnum, 0)),
	TabuTable(vnum, vector<int>(cnum, 0)),
	ColorDomainTable(vnum, vector<bool>(cnum, true)),
	EqualNontabuDeltaU(5000, 0),
	EqualNontabuDeltaV(5000, 0),
	EqualTabuDeltaU(5000, 0),
	EqualTabuDeltaV(5000, 0)
{
	initRand(seed);

	// 初始化节点的邻居和颜色
	for (int i = 0; i < CNum; ++i) {
		for (int j = 0; j < CNum; ++j) {
			for (int t = 0; t < CNum; ++t) {
				if (t != j) { // 行邻居
					RowNeighborTable[i * CNum + j].insert(i * CNum + t); 
					NeighborTable[i * CNum + j].insert(i * CNum + t);
				}
				if (t != i) { // 列邻居
					ColNeighborTable[i * CNum + j].insert(t * CNum + j);
					NeighborTable[i * CNum + j].insert(t * CNum + j);
				}

				ColorDomain[i * CNum + j].insert(t); // 初始化节点的颜色域
			}

			RowColorDomain[i].insert(j); // 初始化行颜色域
		}
	}

	for (auto t : fixNode) { // 初始化固定节点
		int r = t.row, c = t.col, n = t.num;

		int nodeId = r * CNum + c;
		FixNodes[nodeId] = true;
		ColorDomain[nodeId].clear();
		Sol[nodeId] = n;
		RowColorDomain[nodeId / CNum].erase(n); // 删除行颜色域中ci颜色

		for (int i = 0; i < NeighborTable[nodeId].size(); ++i) {
			int u = NeighborTable[nodeId][i];

			ColorDomain[u].erase(n);
		}
	}
}

// 判断节点是否满足缩减规则
int Work::CheckRule(int Node) {
	// Rule1: 当前节点只有一种颜色
	if (ColorDomain[Node].size() == 1) 
		return ColorDomain[Node][0];

	// Rule2: 当前节点在行中有唯一的单独的一种颜色
	BidirectionIndex t1(ColorDomain[Node]);
	for (int i = 0; i < RowNeighborTable[Node].size(); ++i) {
		int t = RowNeighborTable[Node][i];
		for (int j = 0; j < ColorDomain[t].size(); ++j) {
			int c = ColorDomain[t][j];
			t1.erase(c);

		}
		if (!t1.size())
			break;
	}

	if (t1.size() == 1)
		return t1[0];

	// Rule3: 当前节点在列中有唯一的单独的一种颜色
	BidirectionIndex t2(ColorDomain[Node]);
	for (int i = 0; i < ColNeighborTable[Node].size(); ++i) {
		int t = ColNeighborTable[Node][i];
		for (int j = 0; j < ColorDomain[t].size(); ++j) {
			int c = ColorDomain[t][j];
			t2.erase(c);
		}
		if (!t2.size())
			break;
	}

	if (t2.size() == 1)
		return t2[0];

	return -1;
}

void Work::InitSol() {
	// 删除固定点
	unordered_set<int> CandSet; // 候选集
	// 初始化候选集
	for (int i = 0; i < VNum; ++i) 
		if(!FixNodes[i])
			CandSet.insert(i);

	while (!CandSet.empty()) {
		bool flag = false;
		int ci;
		unordered_set<int> Tset;
		for (auto v : CandSet) {
			if (~(ci = CheckRule(v))) { // 判断节点i是否满足缩减条件
				FixNodes[v] = true;
				Sol[v] = ci;
				Tset.insert(v);

				ColorDomain[v].clear(); // 节点v颜色域设置为空
				RowColorDomain[v / CNum].erase(ci); // 删除行颜色域中ci颜色
				// 删除节点v邻居的颜色域中的ci颜色
				for (int i = 0; i < NeighborTable[v].size(); ++i) {
					int u = NeighborTable[v][i];

					ColorDomain[u].erase(ci);
					ColorDomainTable[u][ci] = false;

				}

				flag = true;
			}
		}

		for (auto v : Tset) 
			CandSet.erase(v);

		if (!flag)
			break;
	}

	for (auto v : CandSet) {
		int row = v / CNum;
		int c = rand(RowColorDomain[row].size());
		Sol[v] = RowColorDomain[row][c];

		RowColorDomain[row].erase(Sol[v]);
	}
}

void Work::FindMove(int iter) {
	int EqualNontabuDeltaLen = 0, EqualTabuDeltaLen = 0;
	int tabudelta = 100000, nontabudelta = 100000;
	int tabusdelta = 0, nontabusdelta = 0;

	// 找到最好动作
	for (int i = 0; i < ConflictNodeLen; ++i) {
		int v = ConflictNodes[i];
		int colorv = Sol[v];

		for (int j = 0; j < RowNeighborTable[v].size(); ++j) {
			int u = RowNeighborTable[v][j];
			int coloru = Sol[u];

			int curdelta = AdjColorTable[v][coloru] + AdjColorTable[u][colorv] - AdjColorTable[u][coloru] - AdjColorTable[v][colorv];
			int cursdelta = ColorDomainTable[v][colorv] - ColorDomainTable[v][coloru] + ColorDomainTable[u][coloru] - ColorDomainTable[u][colorv];

			// 禁忌
			if (TabuTable[v][coloru] > iter || TabuTable[u][colorv] > iter) {
				if (curdelta < tabudelta) {
					tabudelta = curdelta;
					tabusdelta = cursdelta;

					EqualTabuDeltaLen = 0;
					EqualTabuDeltaV[EqualTabuDeltaLen] = v;
					EqualTabuDeltaU[EqualTabuDeltaLen] = u;
					++EqualTabuDeltaLen;
				}
				else if (curdelta == tabudelta && cursdelta < tabusdelta) {
					tabusdelta = cursdelta;

					EqualTabuDeltaLen = 0;
					EqualTabuDeltaV[EqualTabuDeltaLen] = v;
					EqualTabuDeltaU[EqualTabuDeltaLen] = u;
					++EqualTabuDeltaLen;
				}
				else if(curdelta == tabudelta && cursdelta == tabusdelta) {
					EqualTabuDeltaV[EqualTabuDeltaLen] = v;
					EqualTabuDeltaU[EqualTabuDeltaLen] = u;
					++EqualTabuDeltaLen;
				}
			}
			else { // 非禁忌
				if (curdelta < nontabudelta) {
					nontabudelta = curdelta;
					nontabusdelta = cursdelta;

					EqualNontabuDeltaLen = 0;
					EqualNontabuDeltaV[EqualNontabuDeltaLen] = v;
					EqualNontabuDeltaU[EqualNontabuDeltaLen] = u;
					++EqualNontabuDeltaLen;
				}
				else if (curdelta == nontabudelta && cursdelta < nontabusdelta) {
					nontabusdelta = cursdelta;

					EqualNontabuDeltaLen = 0;
					EqualNontabuDeltaV[EqualNontabuDeltaLen] = v;
					EqualNontabuDeltaU[EqualNontabuDeltaLen] = u;
					++EqualNontabuDeltaLen;
				}
				else if (curdelta == nontabudelta && cursdelta == nontabusdelta) {
					EqualNontabuDeltaV[EqualNontabuDeltaLen] = v;
					EqualNontabuDeltaU[EqualNontabuDeltaLen] = u;
					++EqualNontabuDeltaLen;
				}
			}
		}
	}

	// 是否满足禁忌特赦
	if (tabudelta < nontabudelta && conflict + tabudelta < besthistoryf) {
		int choice = rand(EqualTabuDeltaLen);

		bestv = EqualTabuDeltaV[choice];
		bestu = EqualTabuDeltaU[choice];
		delta = tabudelta;
	}
	else {
		int choice = rand(EqualNontabuDeltaLen);

		bestv = EqualNontabuDeltaV[choice];
		bestu = EqualNontabuDeltaU[choice];
		delta = nontabudelta;
	}
}


void Work::MakeMove(int iter) {
	int colorv = Sol[bestv], coloru = Sol[bestu];

	Sol[bestv] = coloru;
	Sol[bestu] = colorv;

	conflict += delta;

	if (!conflict)
		return;

	if (conflict < besthistoryf) // 更新历史最好冲突数
		besthistoryf = conflict;
	
	// 更新禁忌表 
	TabuTable[bestv][colorv] = iter + 0.4 * conflict + rand(1, 11);
	if (ConflictNodePos[bestu] != -1)
		TabuTable[bestu][coloru] = iter + 0.4 * conflict + rand(1, 11);

	// 在冲突节点中删除bestv和bestu
	if (AdjColorTable[bestv][coloru] == 0) {
		int tmp = ConflictNodes[ConflictNodeLen - 1];
		ConflictNodes[ConflictNodePos[bestv]] = tmp;
		ConflictNodePos[tmp] = ConflictNodePos[bestv];
		ConflictNodePos[bestv] = -1;
		--ConflictNodeLen;
	}
	if (ConflictNodePos[bestu] != -1 && AdjColorTable[bestu][colorv] == 0) {
		int tmp = ConflictNodes[ConflictNodeLen - 1];
		ConflictNodes[ConflictNodePos[bestu]] = tmp;
		ConflictNodePos[tmp] = ConflictNodePos[bestu];
		ConflictNodePos[bestu] = -1;
		--ConflictNodeLen;
	}
	if(ConflictNodePos[bestu] == -1 && AdjColorTable[bestu][colorv] > 0) {
		ConflictNodes[ConflictNodeLen] = bestu;
		ConflictNodePos[bestu] = ConflictNodeLen;
		++ConflictNodeLen;
	}

	// 更新冲突节点和颜色表
	for (int i = 0; i < ColNeighborTable[bestv].size(); ++i) {
		int t = ColNeighborTable[bestv][i];

		--AdjColorTable[t][colorv];
		++AdjColorTable[t][coloru];

		// 更新颜色表
		if (ConflictNodePos[t] != -1 && AdjColorTable[t][Sol[t]] == 0) {
			int tmp = ConflictNodes[ConflictNodeLen - 1];
			ConflictNodes[ConflictNodePos[t]] = tmp;
			ConflictNodePos[tmp] = ConflictNodePos[t];
			ConflictNodePos[t] = -1;
			--ConflictNodeLen;
		}
		else if (ConflictNodePos[t] == -1 && AdjColorTable[t][Sol[t]] > 0) {
			ConflictNodes[ConflictNodeLen] = t;
			ConflictNodePos[t] = ConflictNodeLen;
			++ConflictNodeLen;
		}
	}

	//ColNeighborTable[bestu].insert(bestu);
	for (int i = 0; i < ColNeighborTable[bestu].size(); ++i) {
		int t = ColNeighborTable[bestu][i];

		--AdjColorTable[t][coloru];
		++AdjColorTable[t][colorv];

		// 更新颜色表
		if (ConflictNodePos[t] != -1 && AdjColorTable[t][Sol[t]] == 0) {
			int tmp = ConflictNodes[ConflictNodeLen - 1];
			ConflictNodes[ConflictNodePos[t]] = tmp;
			ConflictNodePos[tmp] = ConflictNodePos[t];
			ConflictNodePos[t] = -1;
			--ConflictNodeLen;
		}
		else if (ConflictNodePos[t] == -1 && AdjColorTable[t][Sol[t]] > 0) {
			ConflictNodes[ConflictNodeLen] = t;
			ConflictNodePos[t] = ConflictNodeLen;
			++ConflictNodeLen;
		}
	}
	//ColNeighborTable[bestu].erase(bestu);
}

void Work::check() {
	vector<vector<int>> lsc(CNum, vector<int>(CNum));
	for (int i = 0; i < VNum; ++i) {
		int r = i / CNum, c = i % CNum;

		lsc[r][c] = BestSol[i];
	}

	int tot = 0;
	for (int i = 0; i < CNum; ++i) {
		for (int j = 0; j < CNum; ++j) {
			for (int k = 0; k < CNum; ++k) {
				if (k != j && lsc[i][j] == lsc[i][k])
					++tot;
				if (k != i && lsc[i][j] == lsc[k][j])
					++tot;
			}
		}
	}

	if (tot == 0)
		cerr << "\033[32mConflict Numbers: \033[0m" << tot << endl;
	else {
		cerr << "\033[31mConflict Numbers:\033[0m" << tot << endl;
		
		tot /= 2;
		if (tot != besthistoryf)
			cerr << "\033[31mError. tot: \033[0m" << tot << " conflict: " << besthistoryf << endl;
	}
}

int Work::solve(function<long long()> restMilliSec) {
	InitSol();
	
	for (int i = 0; i < VNum; ++i) {
		for (int j = 0; j < ColNeighborTable[i].size(); ++j) { // 计算冲突数
			int t = ColNeighborTable[i][j];

			++AdjColorTable[i][Sol[t]];

			// 冲突节点
			if (Sol[i] == Sol[t]) {
				++ conflict; // 冲突数加一

				if (!FixNodes[i] && ConflictNodePos[i] == -1) {
					ConflictNodes[ConflictNodeLen] = i;
					ConflictNodePos[i] = ConflictNodeLen;
					++ConflictNodeLen;
				}
			}
		}
	}

	if (conflict == 0) {
		BestSol = Sol;
		return 0;
	}

	conflict /= 2;
	besthistoryf = conflict;

	// 删除固定节点
	for (int i = 0; i < VNum; ++i) {
		if (!FixNodes[i])
			continue;

		for (int j = 0; j < RowNeighborTable[i].size(); ++j) {
			int t = RowNeighborTable[i][j];

			RowNeighborTable[t].erase(i);
			NeighborTable[t].erase(i);
		}

		for (int j = 0; j < ColNeighborTable[i].size(); ++j) {
			int t = ColNeighborTable[i][j];

			ColNeighborTable[t].erase(i);
			NeighborTable[t].erase(i);
		}
	}

	int accu = 0, rt = RT0;
	BestSol = Sol;
	int bestconflict = conflict;

	// 恢复BestSol时的冲突节点
	int ConflictNodeLen_;
	vector<int> ConflictNodes_; // 冲突节点
	vector<int> ConflictNodePos_;
	vector<vector<int>> AdjColorTable_; // 邻居颜色表

	int t = -100000, iter;
	for (iter = 0; restMilliSec() > 0; ++iter) {
		FindMove(iter);

		MakeMove(iter);
		
		t = max(t, conflict - bestconflict);
		if (conflict < bestconflict) {
			BestSol = Sol;
			bestconflict = conflict;

			ConflictNodeLen_ = ConflictNodeLen;
			ConflictNodes_ = ConflictNodes;
			ConflictNodePos_ = ConflictNodePos;
			AdjColorTable_ = AdjColorTable;

			if (conflict == 0) {
				return iter;
			}
		}
		if(iter % 100000 == 0) {
		// else if (conflict - bestconflict > rt) {
			Sol = BestSol;
			conflict = bestconflict;

			// 恢复best时冲突节点
			ConflictNodeLen = ConflictNodeLen_;
			ConflictNodes = ConflictNodes_;
			ConflictNodePos = ConflictNodePos_;
			AdjColorTable = AdjColorTable_;

			// 清空禁忌表
			iter = 0;
			TabuTable.assign(VNum, vector<int>(CNum, 0));

			if (rt < MAXRT) {
				++accu;

				if (accu == MAXACCU) {
					accu = 0;
					++rt;
				}
			}
		}
	}

	cerr << "Max Minus: " << t << endl;
	return iter;
}

void Work::savesol(ostream &os) {
	std::cerr << "Save Output." << endl;
	for (int i = 0; i < VNum; ++i) {
		
		os << BestSol[i] << ' ';

		if ((i + 1) % CNum == 0)
			os << endl;
	}
}

namespace szx {

class Solver {
	// random number generator.
	mt19937 pseudoRandNumGen;
	void initRand(int seed) { pseudoRandNumGen = mt19937(seed); }
	int fastRand(int lb, int ub) { return (pseudoRandNumGen() % (ub - lb)) + lb; }
	int fastRand(int ub) { return pseudoRandNumGen() % ub; }
	int rand(int lb, int ub) { return uniform_int_distribution<int>(lb, ub - 1)(pseudoRandNumGen); }
	int rand(int ub) { return uniform_int_distribution<int>(0, ub - 1)(pseudoRandNumGen); }

public:
	void solve(ostream& outputStream, LatinSquare& input, function<long long()> restMilliSec, int seed) {
		// Debug
		steady_clock::time_point StartTime = steady_clock::now();
		Work solver(input.n * input.n, input.n, input.fixedNums, seed);

		int iter = solver.solve(restMilliSec);
		solver.savesol(outputStream);
		
		steady_clock::time_point EndTime = steady_clock::now();
		uint64_t duration = duration_cast<milliseconds>(EndTime - StartTime).count();

		cerr << "Run Time: " << duration << " Frequency: " << iter << endl;
		// solver.check();
	}
};

// solver.
void solveLatinSquare(ostream& outputStream, LatinSquare& input, function<long long()> restMilliSec, int seed) {
	Solver().solve(outputStream, input, restMilliSec, seed);
}

}
