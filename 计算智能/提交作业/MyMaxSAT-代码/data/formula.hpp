//
// Created by liyan on 2019/10/22.
//

#ifndef MYMAXSAT_FORMULA_HPP
#define MYMAXSAT_FORMULA_HPP

#include <algorithm>
#include <utility>
#include "clause.hpp"

class Formula {
public:
    List<Clause> clauses;
    HashMap<ID, bool> variables;

public:
    Formula(List<Clause> cla, HashMap<ID, bool> var) : clauses(std::move(cla)), variables(std::move(var)) {}

    Formula(List<Clause> cla, const List<Variable> &var) : clauses(std::move(cla)) {
        for (auto &v : var) { variables[v.id] = false; }
    }

    Formula() = default;

    List<Clause> getSatisfiedClauses() {
        List<Clause> satisfied_clauses;
        satisfied_clauses.reserve(clauses.size());
        for (auto &c : clauses) {
            bool satisfied = false;
            for (auto &v : c.variables) {
                satisfied |= v.type == Variable::VarType::positive == getVariableValue(v.id);
            }
            if (satisfied)
                satisfied_clauses.push_back(c);
        }
        return satisfied_clauses;
    }

    String toString() const {
        String str = "[";
        for (auto iter = clauses.begin(); iter != clauses.end() - 1; ++iter)
            str += iter->toString() + " /\\ ";
        str += clauses.back().toString() + "]";
        return str;
    }

private:
    inline bool getVariableValue(ID var_id) {
        return variables.at(var_id);
    }

};


#endif //MYMAXSAT_FORMULA_HPP
