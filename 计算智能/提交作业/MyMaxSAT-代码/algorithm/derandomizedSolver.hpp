//
// Created by liyan on 2019/10/22.
//

#ifndef MYMAXSAT_DERANDOMIZEDSOLVER_HPP
#define MYMAXSAT_DERANDOMIZEDSOLVER_HPP

#include <algorithm>
#include <cmath>
#include <cassert>
#include <thread>
#include "baseSolver.hpp"
#include "../MpSolver/Utility.h"

class DerandomizedSolver : virtual public BaseSolver {

public:
    using BaseSolver::BaseSolver;

    void solve() override {
        List<double> p_list(formula.variables.size(), 0.5);
        int calculate_weight = derandomize(p_list);

#pragma region resultChecker
		int check_weight = 0;
		for (const auto &c : formula.getSatisfiedClauses()) { check_weight += c.weight; }
		if (calculate_weight != check_weight) {
			// [TODO] add exception
			// 当 derandomize() 超时，check 失败，check_weight 为真实值
			std::cout << "DerandomizedSolver check failed." 
				<< " calculate_weight: " << calculate_weight
				<< " check_weight: " << check_weight
				<< std::endl;
		}
#pragma endregion resultChecker
    }

protected:
    int derandomize(const List<double> &p_list) {
		szx::Timer timer(static_cast<szx::Timer::Millisecond>(Cfg::DerandomizedTimeoutInSec * 1000)); // 超时检测
       
		List<int> unspecified_num(formula.clauses.size(), 0);   // 剩余未指定变量数
        List<bool> is_satisfied(formula.clauses.size(), false); // 子句是否满足

        expectedGeneral(unspecified_num);
        for (auto &v : formula.variables) {
#if THREAD_ON
			/*
			// [TODO] 使用 std::future 和 std::promise 获取线程返回值
			std::thread threadTrue(&DerandomizedSolver::expectedConditional, this, std::cref(unspecified_num), std::cref(is_satisfied), v.first, true);
			std::thread threadFalse(&DerandomizedSolver::expectedConditional, this, std::cref(unspecified_num), std::cref(is_satisfied), v.first, false);
			*/
			double expectedTrue, expectedFalse;
			std::thread threadTrue([&, this]() {
				expectedTrue = expectedConditional(unspecified_num, is_satisfied, v.first, true) * p_list.at(v.first);
			});
			std::thread threadFalse([&, this]() {
				expectedFalse = expectedConditional(unspecified_num, is_satisfied, v.first, false) * (1 - p_list.at(v.first));
			});
			threadTrue.join();
			threadFalse.join();
#else
			double expectedTrue =
				expectedConditional(unspecified_num, is_satisfied, v.first, true) * p_list.at(v.first);
			double expectedFalse =
				expectedConditional(unspecified_num, is_satisfied, v.first, false) * (1 - p_list.at(v.first));

#endif // THREAD_ON
            if (expectedTrue > expectedFalse) {
                v.second = true;
                expectedUpdate(unspecified_num, is_satisfied, v.first, true);
            } else {
                v.second = false;
                expectedUpdate(unspecified_num, is_satisfied, v.first, false);
            }

			if (timer.isTimeOut()) { break; }
        }

        int total_weight = 0;
        for (int i = 0; i < formula.clauses.size(); ++i) {
            if (is_satisfied[i]) { total_weight += formula.clauses[i].weight; }
        }
        return total_weight;
    }

    // 计算一般期望，同时初始化 unfilled_num
    double expectedGeneral(List<int> &unspecified_num) {
        const auto &clauses = formula.clauses;
        double sum = 0.0;
        for (int i = 0; i < clauses.size(); ++i) {
            unspecified_num[i] = static_cast<int>(clauses[i].variables.size());
            sum += (1 - 1 / std::pow(2, unspecified_num[i])) * clauses[i].weight;
        }
        return sum;
    }

    // 计算条件期望
    double expectedConditional(const List<int> &unspecified_num, const List<bool> &is_satisfied, ID var, bool val) {
        const auto &clauses = formula.clauses;
        double sum = 0.0;
        for (int i = 0; i < clauses.size(); ++i) {
            if (is_satisfied[i]) {  // 子句已满足
                sum += clauses[i].weight;
            }
            if (!is_satisfied[i] && unspecified_num[i]) { // 子句未满足 & 有空变量
                auto iter = std::find_if(clauses[i].variables.begin(), clauses[i].variables.end(),
                                         [=](auto &v) { return v.id == var; });
                if (iter != clauses[i].variables.end()) { // 子句包含当前变量
                    if ((iter->type == Variable::VarType::positive && val) ||
                        (iter->type == Variable::VarType::negative && !val)) {
                        sum += clauses[i].weight;
                    } else {
                        sum += (1 - 1 / std::pow(2, unspecified_num[i] - 1)) * clauses[i].weight;
                    }
                } else { // 子句不包含当前变量
                    sum += (1 - 1 / std::pow(2, unspecified_num[i])) * clauses[i].weight;
                }
            }
        }
        return sum;
    }

    // 更新 unspecified_num & is_satisfied
    void expectedUpdate(List<int> &unspecified_num, List<bool> &is_satisfied, ID var, bool val) {
        const auto &clauses = formula.clauses;
        for (int i = 0; i < clauses.size(); ++i) {
            if (!is_satisfied[i] && unspecified_num[i]) {
                auto iter = std::find_if(clauses[i].variables.begin(), clauses[i].variables.end(),
                                         [=](auto &v) { return v.id == var; });
                if (iter != clauses[i].variables.end()) {
                    --unspecified_num[i];
                    if ((iter->type == Variable::VarType::positive && val) ||
                        (iter->type == Variable::VarType::negative && !val)) {
                        is_satisfied[i] = true;
                    }
                }
            }
        }
    }
};


#endif //MYMAXSAT_DERANDOMIZEDSOLVER_HPP
