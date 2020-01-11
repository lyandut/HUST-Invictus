//
// Created by liyan on 2019/10/24.
//

#ifndef MYMAXSAT_LPSOLVER_HPP
#define MYMAXSAT_LPSOLVER_HPP

#include "../MpSolver/MpSolver.h"
#include "../data/formula.hpp"

namespace szx {

using Dvar = MpSolver::DecisionVar;
using Expr = MpSolver::LinearExpr;

class LPSolver : virtual public BaseSolver {

public:
    using BaseSolver::BaseSolver;

    void solve() override {
        List<double>(LPSolver::*lpFunc)();
#if MP_MODEL
        lpFunc = &LPSolver::mpModel;
#else
        lpFunc = &LPSolver::gurobiModel;
#endif // MP_MODEL

        List<double> p_list = (this->*lpFunc)();
        for (auto &var : formula.variables) {
            var.second = getProbRandomNumber(p_list.at(var.first));
        }
    }

	// 返回 p_list，给 LPDerandomizedSolver 使用，避免重新调用 Gurobi 计算
	void solve(List<double> & p_list) {
		List<double>(LPSolver::*lpFunc)();
#if MP_MODEL
		lpFunc = &LPSolver::mpModel;
#else
		lpFunc = &LPSolver::gurobiModel;
#endif // MP_MODEL

		p_list = (this->*lpFunc)();
		for (auto &var : formula.variables) {
			var.second = getProbRandomNumber(p_list.at(var.first));
		}
	}

protected:
    List<double> mpModel() {
		List<double> p_list(formula.variables.size(), 0.5);
		try {
			/*
			* Initialize environment & empty model
			*/
			MpSolver::Configuration mpCfg(MpSolver::InternalSolver::GurobiMip, Cfg::GRBTimeoutInSec);
			MpSolver mp(mpCfg);
			mp.setMaxThread(Cfg::ThreadNumber);
			//mp.setMaxThread(std::thread::hardware_concurrency());

			/*
			* Decision Variables
			* 1. Bool: y_j correspond to the values of each boolean variable x_j.
			* 2. Bool: q_i correspond to the truth value of each clause C_i.
			* 3. Relax: 0 <= y_i, q_i <= 1.
			*/
			List<Dvar> y(formula.variables.size());
			List<Dvar> q(formula.clauses.size());
			for (const auto &v : formula.variables) {
				y[v.first] = mp.addVar(MpSolver::VariableType::Real, 0, 1);
			}
			for (int i = 0; i < formula.clauses.size(); ++i) {
				q[i] = mp.addVar(MpSolver::VariableType::Real, 0, 1);
			}

			/*
			* Constraint
			* q_i <= Sum(y_j) + Sum(1 - ~y_j)
			*/
			for (int i = 0; i < formula.clauses.size(); ++i) {
				Expr sum_variables = 0;
				for (const auto &v : formula.clauses[i].variables) {
					if (v.type == Variable::VarType::positive) {
						sum_variables += y.at(v.id);
					}
					else {
						sum_variables += (1 - y.at(v.id));
					}
				}
				mp.addConstraint(q.at(i) <= sum_variables);
			}

			/*
			* Objective Function
			* maximize Sum(q_i)
			*/
			Expr obj = 0;
			for (int i = 0; i < formula.clauses.size(); ++i) {
				obj += q.at(i) * formula.clauses.at(i).weight;
			}
			mp.addObjective(obj, MpSolver::OptimaOrientation::Maximize);

			// Optimize model
			mp.optimize();
			std::cout << "Obj: " << mp.getObjectiveValue() << std::endl;
			for (const auto &v : formula.variables) {
				//std::cout << mp.getValue(y.at(v.first)) << std::endl;
				p_list[v.first] = mp.getValue(y.at(v.first));
			}
		}
		catch (GRBException &e) {
			std::cout << "Error code = " << e.getErrorCode() << std::endl;
			std::cout << e.getMessage() << std::endl;
			return p_list;
		}
		catch (...) {
			std::cout << "Exception during optimization." << std::endl;
			return p_list;
		}

        return p_list;
    }

    List<double> gurobiModel() {
        List<double> p_list(formula.variables.size(), 0.5);

        try {
            /*
            * Initialize environment & empty model
            */
            GRBEnv env = GRBEnv(true);
            //env.set("LogFile", "max-sat.log");
            env.start();
            GRBModel gm = GRBModel(env);

            /*
            * Decision Variables
            * 1. Bool: y_j correspond to the values of each boolean variable x_j.
            * 2. Bool: q_i correspond to the truth value of each clause C_i.
            * 3. Relax: 0 <= y_i, q_i <= 1.
            */
            List<GRBVar> y(formula.variables.size());
            List<GRBVar> q(formula.clauses.size());
            for (const auto &v : formula.variables) {
                y[v.first] = gm.addVar(0, 1, 0, GRB_CONTINUOUS);
            }
            for (int i = 0; i < formula.clauses.size(); ++i) {
                q[i] = gm.addVar(0, 1, 0, GRB_CONTINUOUS);
            }

            /*
            * Constraint
            * q_i <= Sum(y_j) + Sum(1 - ~y_j)
            */
            for (int i = 0; i < formula.clauses.size(); ++i) {
                GRBLinExpr sum_variables = 0;
                for (const auto &v : formula.clauses[i].variables) {
                    if (v.type == Variable::VarType::positive) {
                        sum_variables += y.at(v.id);
                    } else {
                        sum_variables += (1 - y.at(v.id));
                    }
                }
                gm.addConstr(q.at(i) <= sum_variables);
            }

            /*
            * Objective Function
            * maximize Sum(q_i * W_i)
            */
            GRBLinExpr obj = 0;
            for (int i = 0; i < formula.clauses.size(); ++i) {
				obj += q.at(i) * formula.clauses.at(i).weight;
            }
            gm.setObjective(obj, GRB_MAXIMIZE);

            // Optimize model
            gm.optimize();
            std::cout << "Obj: " << gm.get(GRB_DoubleAttr_ObjVal) << std::endl;
            for (const auto &v : formula.variables) {
                //std::cout << y.at(v.first).get(GRB_DoubleAttr_X) << std::endl;
                p_list[v.first] = y.at(v.first).get(GRB_DoubleAttr_X);
            }
        }
        catch (GRBException &e) {
            std::cout << "Error code = " << e.getErrorCode() << std::endl;
            std::cout << e.getMessage() << std::endl;
			return p_list;
        }
        catch (...) {
            std::cout << "Exception during optimization." << std::endl;
			return p_list;
        }

        return p_list;
    }
};

}

#endif //MYMAXSAT_LPSOLVER_HPP
