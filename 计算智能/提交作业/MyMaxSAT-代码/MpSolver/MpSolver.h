////////////////////////////////
/// usage : 1.	wrapper for mathematical programming solvers.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SMART_SZX_GATE_REASSIGNMENT_MP_SOLVER_H
#define SMART_SZX_GATE_REASSIGNMENT_MP_SOLVER_H


#include "Config.h"

#include <functional>

#include "MpSolverGurobi.h"


namespace szx {

using MpSolver = MpSolverGurobi;
//using MpSolver = MpSolverOrtools;

template<typename T = MpSolver::DecisionVar>
using IsTrue = std::function<bool(const T&)>;

}


#endif // SMART_SZX_GATE_REASSIGNMENT_MP_SOLVER_H
