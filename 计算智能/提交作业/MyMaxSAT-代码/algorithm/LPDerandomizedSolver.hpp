//
// Created by liyan on 2019/10/26.
//

#ifndef MYMAXSAT_LPDERANDOMIZEDSOLVER_HPP
#define MYMAXSAT_LPDERANDOMIZEDSOLVER_HPP

#include "derandomizedSolver.hpp"
#include "LPSolver.hpp"


namespace szx {

class LPDerandomizedSolver : public DerandomizedSolver, public LPSolver {

public:
    LPDerandomizedSolver(const Formula &_formula) : BaseSolver(_formula) {}

    void solve() override {
        List<double>(LPDerandomizedSolver::*lpFunc)();
#if MP_MODEL
        lpFunc = &LPDerandomizedSolver::mpModel;
#else
        lpFunc = &LPDerandomizedSolver::gurobiModel;
#endif // MP_MODEL

        List<double> p_list = (this->*lpFunc)();
        int calculate_weight = derandomize(p_list);

#pragma region resultChecker
		int check_weight = 0;
		for (const auto &c : formula.getSatisfiedClauses()) { check_weight += c.weight; }
        if (calculate_weight != check_weight) {
            // [TODO] add exception
			std::cout << "LPDerandomizedSolver check failed." << std::endl;
        }
#pragma endregion resultChecker
    }
	 
	// 使用 LPSover 的 p_list，不需要重新调用 Gurobi
	void solve(const List<double> & p_list) {
		int calculate_weight = derandomize(p_list);

#pragma region resultChecker
		int check_weight = 0;
		for (const auto &c : formula.getSatisfiedClauses()) { check_weight += c.weight; }
		if (calculate_weight != check_weight) {
			// [TODO] add exception
			std::cout << "LPDerandomizedSolver check failed." << std::endl;
		}
#pragma endregion resultChecker
	}

};

}

#endif //MYMAXSAT_LPDERANDOMIZEDSOLVER_HPP
