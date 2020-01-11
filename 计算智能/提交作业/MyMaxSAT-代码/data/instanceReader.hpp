//
// Created by liyan on 2019/10/26.
//

#ifndef MYMAXSAT_INSTANCEREADER_HPP
#define MYMAXSAT_INSTANCEREADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include "formula.hpp"

class InstanceReader {
public:
	InstanceReader() : nbvar(0), nbclauses(0), top(-1) {}
	InstanceReader(String year, String type, String inst) : inst_year(year), inst_type(type), inst_name(inst) {
		String filename = Cfg::InstanceFolder + year + '/' + type + '/' + inst;
		
		if (loadInstance(filename))
			std::cout << filename << " load success." << std::endl;
		else 
			std::cout << filename << " load failed." << std::endl;
	}

private:
	bool loadInstance(String filename);

public:
	String inst_year;
	String inst_type;
	String inst_name;
	int nbvar;
	int nbclauses;
	int top;
	Formula formula;
};

bool InstanceReader::loadInstance(String filename) {

#pragma region fileChecker
	std::ifstream file(filename);
	if (!file.is_open()) {
		// [todo] add exception
		std::cout << filename << " open failed." << std::endl;
		return false;
	}

	bool is_unqualified = true;
	char start_char;
	String discard;
	while (!file.eof()) {
		file.get(start_char);
		if (start_char == 'p') {
			file >> discard >> nbvar >> nbclauses >> top;
			is_unqualified = false;
			break;
		}
		else {
			std::getline(file, discard);
		}
	}
	if (is_unqualified) {
		file.close();
		std::cout << filename << " is unqualified." << std::endl;
		// [todo] add exception
		return false;
	}
#pragma endregion fileChecker

#pragma region loadInstance
	HashMap<ID, bool> variables;
	for (int i = 0; i < nbvar; ++i) { variables[i] = false; }

	List<Clause> clauses;
	clauses.reserve(nbclauses);
	for (int i = 0; i < nbclauses; ++i) {
		int weight;  file >> weight;
		List<Variable> vars;
		vars.reserve(nbvar);
		ID var; file >> var;
		while (var != 0) {
			if (var < 0) {
				vars.emplace_back(-var - 1, Variable::VarType::negative);
			}
			else {
				vars.emplace_back(var - 1, Variable::VarType::positive);
			}
			file >> var;
		}
		clauses.emplace_back(vars, weight);
	}
	formula = Formula(clauses, variables);
#pragma endregion loadInstance

	file.close();
	return true;
}



#endif //MYMAXSAT_INSTANCEREADER_HPP