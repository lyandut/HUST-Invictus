//
// Created by liyan on 2019/10/22.
//

#ifndef MYMAXSAT_CLAUSE_HPP
#define MYMAXSAT_CLAUSE_HPP

#include <cassert>
#include <utility>
#include "variable.hpp"

class Clause {
public:
    List<Variable> variables;
    int weight;

public:
    Clause(List<Variable> var, int w) : variables(std::move(var)), weight(w) { assert(w >= 0); }

    Clause(int w) : weight(w) { assert(w >= 0); }

    Clause() : Clause(1) {}

    inline void addVariable(const Variable & var) { variables.push_back(var); }

    String toString(bool show_weight = true) const {
        String str = "(";
        for (auto iter = variables.begin(); iter != variables.end() - 1; ++iter)
            str += iter->toString() + " \\/ ";
        str += variables.back().toString() + ")";
        if (show_weight) { str += "{" + std::to_string(weight) + "}"; }
        return str;
    }

};


#endif //MYMAXSAT_CLAUSE_HPP
