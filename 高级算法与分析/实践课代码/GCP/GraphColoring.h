////////////////////////////////
/// usage : 1.	SDK for graph coloring solver.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef CN_HUST_SZX_NPBENCHMARK_GRAPH_COLORING_H
#define CN_HUST_SZX_NPBENCHMARK_GRAPH_COLORING_H


#include <array>
#include <vector>
#include <functional>


namespace szx {

	using NodeId = int;
	using EdgeId = NodeId;
	using ColorId = NodeId;

	using Edge = std::array<NodeId, 2>; // undirected link.

	using Table = std::vector<std::vector<NodeId>>; // 二维表

	struct TriNum {
		NodeId u;
		ColorId c;
		NodeId delta;
	};

	struct GraphColoring {
		NodeId nodeNum;
		EdgeId edgeNum;
		ColorId colorNum;
		std::vector<Edge> edges;

		Table NeighborTable; // 邻居表
	};

	struct solverNodes {
		int f = 0; // 评价函数
		std::vector<ColorId> nodeColors; // 节点颜色
	};

	using NodeColors = std::vector<ColorId>; // `NodeColors[n]` is the color of node `n`.

	void solveGraphColoring(NodeColors& output, GraphColoring& input, std::function<long long()> restMilliSec, int seed);

}


#endif // CN_HUST_SZX_NPBENCHMARK_GRAPH_COLORING_H
