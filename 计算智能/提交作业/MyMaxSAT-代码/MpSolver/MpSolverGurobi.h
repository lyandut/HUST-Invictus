////////////////////////////////
/// usage : 1.	wrap Gurobi optimizer to be more self documenting and easier to use.
///         2.  configure Gurobi as http://www.gurobi.com/support/faqs instruct
///             (How do I configure a new Gurobi C++ project with Microsoft Visual Studio),
///             then just add MpSolver.h and MpSolver.cpp to your project to use.
///
/// note  : 1.  DO NOT include this file manually, include MpSolver.h instead.
///         2.  this project does not provide Gurobi lisence.
////////////////////////////////

#ifndef SMART_SZX_GATE_REASSIGNMENT_MP_SOLVE_GUROBI_H
#define SMART_SZX_GATE_REASSIGNMENT_MP_SOLVE_GUROBI_H


#include "Config.h"

#include <algorithm>
#include <iostream>
#include <functional>

#include "Common.h"
#include "Utility.h"
#include "MpSolverBase.h"

#include "gurobi_c++.h"


#pragma region AutoLinking
/// only use the major and minor version number, e.g. v6.5.0 => 65, v6.0.5 => 60.
#define GUROBI_VERSION  80

#if _DR_DEBUG
#if _LL_DYNAMIC
#define LINK_TYPE  mdd
#else // _LL_STATIC
#define LINK_TYPE  mtd
#endif // _LL_DYNAMIC
#else // _DR_RELEASE
#if _LL_DYNAMIC
#define LINK_TYPE  md
#else // _LL_STATIC
#define LINK_TYPE  mt
#endif // _LL_DYNAMIC
#endif // _DR_DEBUG

#pragma comment(lib, RESOLVED_STRINGIFY(RESOLVED_CONCAT(gurobi, GUROBI_VERSION)))
#pragma comment(lib, RESOLVED_STRINGIFY(RESOLVED_CONCAT2(gurobi_c++, LINK_TYPE, _CC_VERSION)))
#pragma endregion AutoLinking


namespace szx {

class MpSolverGurobi : public MpSolverBase {
    #pragma region Constant
public:
    // Gurobi variable can be GRB_CONTINUOUS, GRB_BINARY, GRB_INTEGER, GRB_SEMICONT, or GRB_SEMIINT.
    // Semi-continuous variables can take any value between the specified lower and upper bounds, or a value of zero. 
    // Semi-integer variables can take any integer value between the specified lower and upper bounds, or a value of zero
    enum VariableType { Bool = GRB_BINARY, Integer = GRB_INTEGER, Real = GRB_CONTINUOUS, SemiInt = GRB_SEMIINT, SemiReal = GRB_SEMICONT };

    enum OptimaOrientation { Minimize = GRB_MINIMIZE, Maximize = GRB_MAXIMIZE };

    // status for the most recent optimization.
    enum ResultStatus {
        Optimal,         // GRB_OPTIMAL
        Feasible,        // GRB_SUBOPTIMAL || (SolutionCount > 0)
        Proceeding,      // GRB_LOADED || GRB_INPROGRESS
        Ready,           // ready for solve()
        ExceedLimit,     // GRB_ITERATION_LIMIT || GRB_NODE_LIMIT || GRB_TIME_LIMIT || GRB_SOLUTION_LIMIT
        InsolubleCutoff, // GRB_CUTOFF
        InsolubleModel,  // GRB_INFEASIBLE || GRB_INF_OR_UNBD || GRB_UNBOUNDED
        OutOfMemory,     // OUT_OF_MEMORY
        Error            // any other status code or error code (you may not get it as exception is thrown)
    };

    // behavior of searching or recording alternative solutions.
    enum PoolingMode {
        Incidental = 0, // only keeps solutions found along the way to the optimum incidentally.
        Good = 1,       // keep searching for high quality solutions after the optimum is found.
        Top = 2,        // search top K solutions in a systematic way.
        DefaultPoolingMode = Incidental
    };

    enum MipFocusMode {
        ImproveFeasibleSolution = 1, // find sub-optima as fast as possible.
        ProveOptimality = 2,         // reach and prove the optima is mandatory.
        ImproveBound = 3,            // improve the bound as much as possible.
        DefaultFocus = 0             // balance between modes.
    };

    enum SymmetryDetectionMode {
        NoDetection = 0,
        ConservativeDetection = 1,
        AggressiveDetection = 2,
        DefaultDetectionMode = -1
    };

    enum PresolveLevel {
        NoPresolve = 0,
        ConservativePresolve = 1,
        AggressivePresolve = 2,
        DefaultPresolveMode = -1
    };

    enum IisMethod {
        FastIis = 0,
        SmallIis = 1,
        IgnoreBoundIis = 2, // ignores the bound constraints.
        LpRelaxIis = 3,     // the IIS for the LP relaxation of a MIP model if the relaxation is infeasible, even though the result may not be minimal when integrality constraints are included. 
        DefaultIisMethod = -1
    };

    static constexpr int MaxInt = GRB_MAXINT;
    static constexpr double MaxReal = GRB_INFINITY;
    static constexpr double Infinity = GRB_INFINITY;
    static constexpr double Undefined = GRB_UNDEFINED;

    static constexpr int AutoThreading = 0; // default thread number to let Gurobi make the decision.

    static constexpr int DefaultObjectivePriority = 0;
    static constexpr OptimaOrientation DefaultObjectiveOptimaOrientation = Maximize;

    static constexpr double MillisecondsPerSecond = 1000;

    static constexpr int MaxObjectivePriority = 32;

    static constexpr auto DefaultParameterPath = "tune.prm";
    static constexpr auto DefaultIrreducibleInconsistentSubsystemPath = "iis.ilp";
    #pragma endregion Constant

    #pragma region Type
public:
    class MpEvent;

    using DecisionVar = GRBVar;
    using Constraint = GRBConstr;
    using LinearExpr = GRBLinExpr;
    using LinearRange = GRBTempConstr;

    using Millisecond = long long;

    // on solving single objective during multi-objective optimization.
    using OnSolveBegin = std::function<void(void)>;
    // after solving single objective during multi-objective optimization.
    // returns true if obj cut-off is already added.
    using OnOptimaFound = std::function<bool(MpSolverGurobi&, std::function<bool(void)>)>;
    // on finding an MIP solution during optimization.
    using OnMipSln = std::function<void(MpEvent&)>;

    struct Configuration {
        static constexpr InternalSolver DefaultSolver = InternalSolver::GurobiMip;

        static constexpr double DefaultObjectiveRelativeTolerance = 0;
        static constexpr double DefaultObjectiveAbsoluteTolerance = 0;

        static constexpr double DefaultObjectiveWeightRadix = 100;
        static constexpr int DefaultObjectiveWeightOffset = -1; // the weight of the objective with the lowest priority is 10^(-1).

        static constexpr bool DefaultOutputState = false;

        static constexpr bool DefaultMultiObjMode = true; // true for priority, false for weight.
        static constexpr bool EnableCallbackForEachObj = true; // allow preprocess/postprocess for each obj in priority mode.

        static constexpr double Forever = MaxInt;

        Configuration(InternalSolver type = DefaultSolver, double timeoutInSec = Forever,
            bool usePriorityMode = Configuration::DefaultMultiObjMode, bool shouldEnableOutput = DefaultOutputState)
            : internalSolver(type), timeoutInSecond(timeoutInSec), inPriorityMode(usePriorityMode), enableOutput(shouldEnableOutput) {}

        friend std::ostream& operator<<(std::ostream &os, const Configuration &cfg) {
            return os << "grb" << "." << (cfg.inPriorityMode ? "P" : "W");
        }

        InternalSolver internalSolver;
        double timeoutInSecond; // total timeout.
        bool inPriorityMode; // or in weight mode.
        bool enableOutput;
    };

    struct SubObjective {
        LinearExpr expr;
        OptimaOrientation optimaOrientation;
        int index;
        int priority;
        double relTolerance;
        double absTolerance;
        double timeoutInSecond; // this will overwrite total timeout if it is greater than 0.
        OnOptimaFound postprocess; // invoked after this sub-objective is solved in priority mode.
        OnSolveBegin preprocess; // invoked before this sub-objective begin solving in priority mode.
    };

    class MpEvent : public GRBCallback {
    protected:
        friend MpSolverGurobi;

        MpEvent(OnMipSln onMipSolutionFound = OnMipSln())
            : onMipSln(onMipSolutionFound) {}

    public:
        using GRBCallback::addCut;
        using GRBCallback::addLazy;
        void stop() { abort(); }

        double getValue(const DecisionVar &var) { return getSolution(var); }
        double getValue(const LinearExpr &expr) {
            double value = expr.getConstant();
            int itemNum = static_cast<int>(expr.size());
            for (int i = 0; i < itemNum; ++i) {
                value += (expr.getCoeff(i) * getValue(expr.getVar(i)));
            }
            return value; // OPTIMIZE[szx][9]: try `return expr.getValue();`?
        }
        bool isTrue(const DecisionVar &var) { return (getValue(var) > 0.5); }
        double getRelaxedValue(const DecisionVar &var) { return getNodeRel(var); }

        void setValue(DecisionVar &var, double value) { setSolution(var, value); }
        //using GRBCallback::useSolution;

        double getObj() { return getDoubleInfo(GRB_CB_MIPSOL_OBJ); }
        double getBestObj() {
            switch (where) {
            case GRB_CB_MIP:
                getDoubleInfo(GRB_CB_MIP_OBJBST);
                break;
            case GRB_CB_MIPNODE:
                getDoubleInfo(GRB_CB_MIPNODE_OBJBST);
                break;
            case GRB_CB_MIPSOL:
                getDoubleInfo(GRB_CB_MIPSOL_OBJBST);
                break;
            }
        }

        void callback() {
            if (where == GRB_CB_MIPSOL) {
                if (onMipSln) { onMipSln(*this); }
            }
        }

        OnMipSln onMipSln;
    };
    #pragma endregion Type

    #pragma region Constructor
public:
    MpSolverGurobi();
    MpSolverGurobi(Configuration &config);

    void loadModel(const String &inputPath) { model.read(inputPath); }
    void saveModel(const String &outputPath) {
        updateModel();
        model.write(outputPath);
    }

    void loadParameter(const String &inputPath = DefaultParameterPath) { model.read(inputPath); }
    void saveParameter(const String &outputPath = DefaultParameterPath) {
        int resultcount = model.get(GRB_IntAttr_TuneResultCount);
        if (resultcount > 0) {
            model.getTuneResult(0);
            model.write(outputPath);
        }
    }
    #pragma endregion Constructor

    #pragma region Method
public:
    bool optimize();

    void tune(const String &outputPath = DefaultParameterPath) {
        try {
            model.set(GRB_IntParam_TuneResults, 1);
            model.tune();
            saveParameter(outputPath);
        } catch (GRBException&) {
            status = ResultStatus::Error;
        }
    }

    void computeIIS(const String &outputPath = DefaultIrreducibleInconsistentSubsystemPath) {
        updateModel();
        try {
            model.set(GRB_IntParam_IISMethod, IisMethod::SmallIis);
            model.computeIIS();
            model.write(outputPath);
        } catch (GRBException&) {
            status = ResultStatus::Error;
        }
    }

    // status.
    static bool reportStatus(ResultStatus status);
    Millisecond getDuration() const { return static_cast<Millisecond>(timer.elapsedSeconds() * MillisecondsPerSecond); }

    // decisions.
    DecisionVar addVar(VariableType type, double lb = 0, double ub = 1, double objCoef = 0, const String &name = "") {
        return model.addVar(lb, ub, objCoef, static_cast<char>(type), name);
    }

    double getValue(const LinearExpr &expr) const { return expr.getValue(); }
    double getValue(const DecisionVar &var) const { return var.get(GRB_DoubleAttr_X); }
    double getAltValue(const DecisionVar &var, int solutionIndex) {
        setAltSolutionIndex(solutionIndex);
        return var.get(GRB_DoubleAttr_Xn);
    }

    using MpSolverBase::isTrue;
    bool isTrue(LinearExpr expr) const { return isTrue(getValue(expr)); }
    bool isTrue(DecisionVar var) const { return isTrue(getValue(var)); }
    int getVariableCount() const { return model.get(GRB_IntAttr_NumVars); }

    Arr<DecisionVar> getAllVars() const {
        return Arr<DecisionVar>(getVariableCount(), model.getVars());
    }
    void getAllValues(const Arr<DecisionVar> &vars, Arr<double> &values) const {
        values.init(vars.size());
        auto val = values.begin();
        for (auto var = vars.begin(); var != vars.end(); ++var, ++val) { *val = getValue(*var); }
    }
    void setAllInitValues(Arr<DecisionVar> &vars, const Arr<double> &values) {
        auto val = values.begin();
        for (auto var = vars.begin(); var != vars.end(); ++var, ++val) { setInitValue(*var, *val); }
    }

    int getSolutionCount() const { return model.get(GRB_IntAttr_SolCount); }

    double getPoolObjBound() const { return model.get(GRB_DoubleAttr_PoolObjBound); }

    // constraints.
    Constraint addConstraint(const LinearRange &r, const String &name = "") { return model.addConstr(r, name); }
    void removeConstraint(Constraint constraint) { model.remove(constraint); }
    int getConstraintCount() const { return model.get(GRB_IntAttr_NumConstrs); }

    // objectives.
    void addObjective(const LinearExpr &expr, OptimaOrientation orientation, int priority = DefaultObjectivePriority,
        double relTolerance = Configuration::DefaultObjectiveRelativeTolerance, double absTolerance = Configuration::DefaultObjectiveAbsoluteTolerance,
        double timeoutInSecond = Configuration::Forever, OnOptimaFound postprocess = OnOptimaFound(), OnSolveBegin preprocess = OnSolveBegin()) {
        int index = getObjectiveCount();
        objectives.push_back({ expr, orientation, index, priority, relTolerance, absTolerance, timeoutInSecond, postprocess, preprocess });
    }
    void clearObjectives() { objectives.clear(); }

    double getObjectiveValue() const { return model.get(GRB_DoubleAttr_ObjVal); }
    double getAltObjectiveValue(int solutionIndex) {
        setAltSolutionIndex(solutionIndex);
        return model.get(GRB_DoubleAttr_PoolObjVal);
    }
    double getSubObjectiveValue(int objIndex) {
        setSubObjectiveIndex(objIndex);
        return model.get(GRB_DoubleAttr_ObjNVal);
    }
    double getAltSubObjectiveValue(int solutionIndex, int objIndex) {
        throw MpException("gurobi doesn't support solution pool in a multi-objective model yet.");
        setAltSolutionIndex(solutionIndex);
        return getSubObjectiveValue(objIndex);
    }
    double getObjectiveValue(const SubObjective& subObj) const { return getValue(subObj.expr); }
    List<double> getObjectiveValues() const;

    int getObjectiveCount() const { return static_cast<int>(objectives.size()); }

    // configurations.
    void setTimeLimit(Millisecond millisecond) { setTimeLimitInSecond(millisecond / MillisecondsPerSecond); }
    void setTimeLimitInSecond(double second) {
        double timeout = (std::max)(second, 0.0);
        timer = Timer(Timer::toMillisecond(timeout));
        model.set(GRB_DoubleParam_TimeLimit, timeout);
    }
    // only use it in gurobi multi-obj mode.
    void setTimeLimitInSecond(int objIndex, double second) { model.getMultiobjEnv(objIndex).set(GRB_DoubleParam_TimeLimit, (std::max)(second, 0.0)); }

    void setOutput(bool enable = Configuration::DefaultOutputState) { model.set(GRB_IntParam_OutputFlag, enable); }

    // the methods in MpSolver is invalid within the callback, only use the ones in MpEvent instead.
    void setMipSlnEvent(OnMipSln onMipSln) {
        model.set(GRB_IntParam_LazyConstraints, 1);
        mpEvent.onMipSln = onMipSln;
        model.setCallback(&mpEvent);
    }

    // [Tune] use the given value as the initial solution in MIP.
    void setInitValue(DecisionVar &var, double value) { var.set(GRB_DoubleAttr_Start, value); }
    // [Tune] guide the solver to prefer certain value on certain variable.
    void setHintValue(DecisionVar &var, double value) { var.set(GRB_DoubleAttr_VarHintVal, value); }
    void setHintPrioriy(DecisionVar &var, int priority) { var.set(GRB_IntAttr_VarHintPri, priority); }
    // [Tune] guide the solver to prefer certain variable for branching.
    void setBranchPriority(DecisionVar &var, int priority) { var.set(GRB_IntAttr_BranchPriority, priority); }

    // [Tune] prefer finding good feasible solutions, proving optimality or improving bound.
    void setMipFocus(MipFocusMode mode) { model.set(GRB_IntParam_MIPFocus, mode); }
    // [Tune] the effort on symmetry detection.
    void setSymmetryDetectionMode(SymmetryDetectionMode mode) { model.set(GRB_IntParam_Symmetry, mode); }
    // [Tune] the effort on presolve.
    void setPresolveLevel(PresolveLevel level) { model.set(GRB_IntParam_Presolve, level); }

    void setPoolingMode(PoolingMode poolingMode) { model.set(GRB_IntParam_PoolSearchMode, poolingMode); }
    void setMaxSolutionPoolSize(int maxSolutionNum) { model.set(GRB_IntParam_PoolSolutions, maxSolutionNum); }
    void setMaxSolutionRelPoolGap(double maxRelPoolGap) { model.set(GRB_DoubleParam_PoolGap, maxRelPoolGap); }

    void setMaxThread(int threadNum = AutoThreading) { model.set(GRB_IntParam_Threads, threadNum); }

    void setSeed(int seed) { model.set(GRB_IntParam_Seed, (seed & (std::numeric_limits<int>::max)())); }

protected:
    GRBEnv& getGlobalEnv() {
        thread_local static bool initialized = false;
        if (!initialized) {
            globalEnv.start();
            initialized = true;
        }
        return globalEnv;
    }

    bool optimizeWithGurobiMultiObjective();
    bool optimizeWithManualMultiObjective();
    bool optimizeInPriorityMode(bool useGurobiMultiObjectiveMode = !Configuration::EnableCallbackForEachObj);
    bool optimizeInWeightMode(double radix = Configuration::DefaultObjectiveWeightRadix, int offset = Configuration::DefaultObjectiveWeightOffset);

    void setOptimaOrientation(OptimaOrientation optimaOrientation = DefaultObjectiveOptimaOrientation) {
        model.set(GRB_IntAttr_ModelSense, optimaOrientation);
    }

    void setObjective(const LinearExpr &expr, OptimaOrientation orientation) {
        model.setObjective(expr, orientation);
    }
    void setSubObjective(const SubObjective subObj) {
        LinearExpr expr;
        int priority = MaxObjectivePriority - subObj.priority;
        if (subObj.optimaOrientation == DefaultObjectiveOptimaOrientation) { expr = subObj.expr; } else { expr = (-subObj.expr); }
        model.setObjectiveN(expr, subObj.index, priority, 1, subObj.absTolerance, subObj.relTolerance);
    }
    // determines which sub-objective will be retrieved when calling model.get(GRB_DoubleAttr_ObjNVal).
    void setSubObjectiveIndex(int objIndex) { model.set(GRB_IntParam_ObjNumber, objIndex); }

    // determines which alternative solution will be retrieved when calling var.get(GRB_DoubleAttr_Xn).
    void setAltSolutionIndex(int solutionIndex) { model.set(GRB_IntParam_SolutionNumber, solutionIndex); }

    bool isConstant(const LinearExpr &expr) { return (expr.size() == 0); }
    void updateModel() { model.update(); }

    ResultStatus solve() {
        try {
            model.optimize();
            updateStatus();
        } catch (GRBException &e) {
            if (e.getErrorCode() == GRB_ERROR_OUT_OF_MEMORY) {
                status = ResultStatus::OutOfMemory;
            } else {
                status = ResultStatus::Error;
            }
        }
        return status;
    }

    void updateStatus();
    #pragma endregion Method

    #pragma region Field
public:
protected:
    thread_local static GRBEnv globalEnv;

    // definition of the problem to solve.
    GRBModel model;
    MpEvent mpEvent;

    Configuration cfg;

    // status for the most recent optimization.
    ResultStatus status;
    List<SubObjective> objectives;

public: // fields that rely on initialized cfg.
    Timer timer;
    Timer subObjTimer;
    #pragma endregion Field
}; // MpSolverGurobi

}


#endif // SMART_SZX_GATE_REASSIGNMENT_MP_SOLVE_GUROBI_H
