//
// Created by liyan on 2019/10/25.
//

#ifndef MYMAXSAT_BASESOLVER_HPP
#define MYMAXSAT_BASESOLVER_HPP

#include <iostream>
#include <random>
#include <ctime>
#include "../data/formula.hpp"

class BaseSolver {

public:
	Formula formula;
private:
	std::default_random_engine e;

public:
	BaseSolver(const Formula &_formula) : formula(_formula) { e.seed(time(nullptr)); }

	BaseSolver() = default;

	virtual void solve() = 0;

	int getResult(bool weight_soft_only = true, bool print_flag = false) {
		int total_weight = 0;
		for (const auto &c : formula.getSatisfiedClauses()) {
			if (weight_soft_only && c.weight != Cfg::SoftClauseWeight) { continue; }
			total_weight += c.weight;
		}	

		if (print_flag) {
			std::cout << formula.toString() << std::endl;
			for (const auto &v : formula.variables)
				std::cout << "X" << v.first << ": " << v.second << std::endl;
			std::cout << "Total value: " << total_weight << std::endl;
		}
		
		return total_weight;
	}

protected:
	bool getProbRandomNumber(double __p__ = 0.5) {
		std::bernoulli_distribution u(__p__);
		return u(e);
	}

};


#endif //MYMAXSAT_RANDOMIZEDSOLVER_HPP
