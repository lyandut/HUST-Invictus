#ifndef __HEAD_H__
#define __HEAD_H__

#include <vector>
#include <random>
#include <iostream>
#include <algorithm>
#include <functional>

using NodeColors = int*;

class head {
private:
	int NodeNum, EdgeNum, ColorNum;
	int MAXITER; // 禁忌迭代次数
	int COPYSIZE;
	int SETSIZE;
	int seed;

	std::vector<std::array<int, 2>> edges;
	int** TabuTable; // 禁忌表
	int** ACTable; // 邻居颜色表
	int** NeighborTable; // 节点邻居表
	int* NeighborNums; // 节点邻居的个数

	int* ConflictNodes;
	int* ConflictNodePos;
	int ConflictNodeLen;

	int besthistoryf; // 历史最好的f
	int* EqualNontabuDeltaU, *EqualNontabuDeltaC; // 最好非禁忌动作节点和颜色
	int* EqualTabuDeltaU, *EqualTabuDeltaC; // 最好禁忌动作节点和颜色
	int bestc, bestu, delta;

	NodeColors p1, p2, e1, e2, best; // 结果
	int p1f, p2f, e1f, e2f, bestf; // f

	std::function<long long()> restMilliSec; // 剩余时间
	std::mt19937 pseudoRandNumGen; // 随机数

	inline int rand(int u) { return pseudoRandNumGen() % u; };
	void InitNodeColor(NodeColors sol);
	int ComputeConflict(NodeColors sol);
	void GPX(NodeColors p1, NodeColors p2, NodeColors son);
	void FindMove(NodeColors sol, int &f, int iter);
	void MakeMove(NodeColors sol, int &f, int iter);
	int TabuCol(NodeColors sol);
	bool check(NodeColors p1, NodeColors p2);
public:
	head() = default;
	head(int input_num_vertex, int input_edge_num, int input_num_color, int tabu_max_iter, std::vector<std::array<int, 2>>& input_edges, int input_seed, std::function<long long()> input_restMilliSec);
	~head();

	void HybridEvolution();
	void WriteSolution(std::vector<int>& output);
};




#endif
