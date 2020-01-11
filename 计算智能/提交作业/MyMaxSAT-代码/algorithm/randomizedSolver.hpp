//
// Created by liyan on 2019/10/22.
//

#ifndef MYMAXSAT_RANDOMIZEDSOLVER_HPP
#define MYMAXSAT_RANDOMIZEDSOLVER_HPP

#include "baseSolver.hpp"

class RandomizedSolver : public BaseSolver {

public:
	using BaseSolver::BaseSolver;

    void solve() override { randomize(); }

private:
    void randomize(double __p__ = 0.5) {
		for (auto &var : formula.variables) {
			var.second = getProbRandomNumber(__p__);
		}
    }
};


#endif //MYMAXSAT_RANDOMIZEDSOLVER_HPP
