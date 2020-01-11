#include "MpSolverGurobi.h"

#include "LogSwitch.h"


using namespace std;


namespace szx {

thread_local GRBEnv MpSolverGurobi::globalEnv(true);

MpSolverGurobi::MpSolverGurobi() : model(getGlobalEnv()), status(ResultStatus::Ready),
    timer(Timer::toMillisecond(cfg.timeoutInSecond)), subObjTimer(0ms) {}

MpSolverGurobi::MpSolverGurobi(Configuration &config) : model(getGlobalEnv()), cfg(config),
    status(ResultStatus::Ready), timer(Timer::toMillisecond(config.timeoutInSecond)), subObjTimer(0ms) {
    if (cfg.timeoutInSecond < Configuration::Forever) { setTimeLimitInSecond(cfg.timeoutInSecond); }
    setOutput(cfg.enableOutput);
}

void MpSolverGurobi::updateStatus() {
    switch (model.get(GRB_IntAttr_Status)) {
    case GRB_OPTIMAL:
        status = ResultStatus::Optimal; break;
    case GRB_SUBOPTIMAL:
        status = ResultStatus::Feasible; break;
    case GRB_LOADED:
    case GRB_INPROGRESS:
        status = ResultStatus::Proceeding; break;
    case GRB_ITERATION_LIMIT:
    case GRB_NODE_LIMIT:
    case GRB_TIME_LIMIT:
    case GRB_SOLUTION_LIMIT:
        status = ResultStatus::ExceedLimit; break;
    case GRB_CUTOFF:
        status = ResultStatus::InsolubleCutoff; break;
    case GRB_INFEASIBLE:
    case GRB_INF_OR_UNBD:
    case GRB_UNBOUNDED:
        status = ResultStatus::InsolubleModel; break;
    default:
        status = ResultStatus::Error; break;
    }
    if (model.get(GRB_IntAttr_SolCount) > 0) { status = ResultStatus::Feasible; }
}

bool MpSolverGurobi::reportStatus(ResultStatus status) {
    switch (status) {
    case Optimal:
    case Feasible:
        return true;
    case Proceeding:
    case Ready:
        Log(LogSwitch::Szx::MpSolver) << "Unsolved." << endl; break;
    case InsolubleCutoff:
    case InsolubleModel:
        Log(LogSwitch::Szx::MpSolver) << "Infeasible." << endl; break;
    case ExceedLimit:
        Log(LogSwitch::Szx::MpSolver) << "Exceed Limit." << endl; break;
    case OutOfMemory:
        Log(LogSwitch::Szx::MpSolver) << "Out Of Memory." << endl; break;
    case Error:
    default:
        Log(LogSwitch::Szx::MpSolver) << "Unknown Error." << endl; break;
    }
    return false;
}

List<double> MpSolverGurobi::getObjectiveValues() const {
    List<double> objValues;
    objValues.reserve(getObjectiveCount());
    for (auto i = objectives.begin(); i != objectives.end(); ++i) {
        objValues.push_back(getObjectiveValue(*i));
    }
    return objValues;
}

bool MpSolverGurobi::optimizeWithGurobiMultiObjective() {
    double totalTimeoutInSecond = 0;
    auto i = objectives.begin();
    // single objective optimization.
    if (getObjectiveCount() == 1) {
        setObjective(i->expr, i->optimaOrientation);
        setTimeLimitInSecond((i->timeoutInSecond > 0) ? min(i->timeoutInSecond, timer.restSeconds()) : timer.restSeconds());
    } else {
        // multiple objective optimization.
        setOptimaOrientation();
        for (; i != objectives.end(); ++i) {
            totalTimeoutInSecond += max(i->timeoutInSecond, 0.0);
            setSubObjective(*i);
            setTimeLimitInSecond(i->index, (i->timeoutInSecond > 0) ? min(i->timeoutInSecond, timer.restSeconds()) : timer.restSeconds());
        }
    }

    return reportStatus(solve());
}

bool MpSolverGurobi::optimizeWithManualMultiObjective() {
    List<int> objOrders; // objectives[objOrders[i]] is the i_th prioritized objective.
    int objCount = getObjectiveCount();
    objOrders.resize(objCount);
    for (int i = 0; i < objCount; ++i) { objOrders[i] = i; }
    sort(objOrders.begin(), objOrders.end(),
        [this](int l, int r) { return (objectives[l].priority < objectives[r].priority); });

    bool isSolved = false; // in case all objectives are constant which will be skipped.
    for (auto o = objOrders.begin(); o != objOrders.end(); ++o) {
        SubObjective &subObj(objectives[*o]);
        if (isConstant(subObj.expr)) {
            Log(LogSwitch::Szx::MpSolver) << "obj[" << subObj.priority << "].opt = " << subObj.expr.getValue() << endl;
            continue;
        }
        double restSeconds = (subObj.timeoutInSecond > 0) ? min(subObj.timeoutInSecond, timer.restSeconds()) : timer.restSeconds();
        if (restSeconds <= 0) { return reportStatus(status); }
        setTimeLimitInSecond(restSeconds);
        subObjTimer = Timer(Timer::toMillisecond(restSeconds));
        isSolved = true;
        setObjective(subObj.expr, subObj.optimaOrientation);
        if (subObj.preprocess) { subObj.preprocess(); }

        if (!reportStatus(solve())) { return (o != objOrders.begin()); }
        bool isObjCutOffAdded = subObj.postprocess
            && subObj.postprocess(*this, [&]() { return reportStatus(solve()); });

        double optimalValue = getObjectiveValue();
        Log(LogSwitch::Szx::MpSolver) << "obj[" << subObj.priority << "].opt = " << optimalValue << endl;

        if ((o + 1) == objOrders.end()) { break; }

        if (isObjCutOffAdded) { continue; }
        double tolerance = max(abs(optimalValue * subObj.relTolerance), subObj.absTolerance);
        if (subObj.optimaOrientation == Maximize) {
            addConstraint(subObj.expr >= optimalValue - tolerance);
        } else if (subObj.optimaOrientation == Minimize) {
            addConstraint(subObj.expr <= optimalValue + tolerance);
        }
    }
    if (!isSolved) { return reportStatus(solve()); }
    return true;
}

bool MpSolverGurobi::optimizeInPriorityMode(bool useGurobiMultiObjectiveMode) {
    return (useGurobiMultiObjectiveMode ? optimizeWithGurobiMultiObjective() : optimizeWithManualMultiObjective());
}

bool MpSolverGurobi::optimizeInWeightMode(double radix, int offset) {
    LinearExpr objectiveExpr = 0;
    List<int> objOrders; // objectives[objOrders[i]] is the i_th prioritized objective.
    int objCount = getObjectiveCount();
    objOrders.resize(objCount);
    for (int i = 0; i < objCount; ++i) { objOrders[i] = i; }
    sort(objOrders.begin(), objOrders.end(),
        [this](int l, int r) { return (objectives[l].priority < objectives[r].priority); });

    int i = objCount + offset - 1;
    for (auto o = objOrders.begin(); o != objOrders.end(); ++o, --i) {
        SubObjective &subObj(objectives[*o]);
        double weight = pow(radix, i);
        if (subObj.optimaOrientation == Maximize) {
            objectiveExpr += (weight * subObj.expr);
        } else if (subObj.optimaOrientation == Minimize) {
            objectiveExpr -= (weight * subObj.expr);
        }
    }
    setObjective(objectiveExpr, Maximize);
    setTimeLimitInSecond(cfg.timeoutInSecond);
    return reportStatus(solve());
}

bool MpSolverGurobi::optimize() {
    // non-objective optimization.
    if (objectives.empty()) { return reportStatus(solve()); }

    // single/multi-objective optimization.
    Log(LogSwitch::Szx::MpSolver) << "objectives.size() = " << getObjectiveCount() << endl;

    return (cfg.inPriorityMode ? optimizeInPriorityMode() : optimizeInWeightMode());
}

}