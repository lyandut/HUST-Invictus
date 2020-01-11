////////////////////////////////
/// usage : 1.	interface for mathematical programming solvers.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SMART_SZX_GATE_REASSIGNMENT_MP_SOLVER_BASE_H
#define SMART_SZX_GATE_REASSIGNMENT_MP_SOLVER_BASE_H


#include "Config.h"

#include <exception>
#include <stdexcept>
#include <string>


namespace szx {

using MpException = std::runtime_error;

class MpSolverBase {
public:
    enum InternalSolver {
        ScipMip = 0,
        CbcMip = 0,
        GurobiMip = 1,
        ClpLp = 0,
        GlopLp = 0,
        GurobiLp = 1,
    };


    struct Name {
        static constexpr char Delimiter = '_';


        static std::string str(const char *obj) { return std::string(obj); }

        template<typename T>
        static std::string str(const T &obj) { return std::to_string(obj); }

        template<typename T, typename ... Ts>
        static std::string str(const T &obj, Ts ... objs) { return str(obj) + Delimiter + str(objs ...); }
    };


    static bool isTrue(double value) { return (value > 0.5); }
};

}


#endif // SMART_SZX_GATE_REASSIGNMENT_MP_SOLVER_BASE_H
