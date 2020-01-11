#include <iostream>
#include <ctime>
#include "algorithm/randomizedSolver.hpp"
#include "algorithm/derandomizedSolver.hpp"
#include "algorithm/LPSolver.hpp"
#include "algorithm/LPDerandomizedSolver.hpp"
#include "data/instanceReader.hpp"

void test() {
	List<Variable> variables = { {0}, {1} };
	List<Clause> clauses;

	Clause clause(10);
	clause.addVariable(variables[0]);
	clause.addVariable(variables[1]);
	clauses.push_back(clause);

	clause = Clause(15);
	clause.addVariable(variables[0]);
	clause.addVariable({ variables[1].id, Variable::VarType::negative });
	clauses.push_back(clause);

	clause = Clause(5);
	clause.addVariable({ variables[0].id, Variable::VarType::negative });
	clause.addVariable(variables[1]);
	clauses.push_back(clause);

	clause = Clause(20);
	clause.addVariable({ variables[0].id, Variable::VarType::negative });
	clause.addVariable({ variables[1].id, Variable::VarType::negative });
	clauses.push_back(clause);

	Formula f(clauses, variables);

	std::cout << "=== 1. Randomized Solver ===" << std::endl;
	RandomizedSolver randomizedSolver(f);
	randomizedSolver.solve();
	randomizedSolver.getResult(false, true);

	std::cout << "=== 2. Derandomized Solver ===" << std::endl;
	DerandomizedSolver derandomizedSolver(f);
	derandomizedSolver.solve();
	derandomizedSolver.getResult(false, true);

	using namespace szx; 
	std::cout << "=== 3. LP Solver ===" << std::endl;
	LPSolver lpSolver(f);
	lpSolver.solve();
	lpSolver.getResult(false, true);

	std::cout << "=== 4. LPDerandomized Solver ===" << std::endl;
	LPDerandomizedSolver lpDerandomizedSolver(f);
	lpDerandomizedSolver.solve();
	lpDerandomizedSolver.getResult(false, true);
}

void runSingleInstance(String year, String type, String inst) {
	InstanceReader inst_reader(year, type, inst);
	const auto & f = inst_reader.formula;
	std::clock_t start;

	std::cout << "=== 1. Randomized Solver ===" << std::endl;
	RandomizedSolver randomizedSolver(f);
	start = std::clock();
	randomizedSolver.solve();
	double duration1 = (std::clock() - start) / static_cast<double>(CLOCKS_PER_SEC);
	int alg1 = randomizedSolver.getResult();

	std::cout << "=== 2. Derandomized Solver ===" << std::endl;
	DerandomizedSolver derandomizedSolver(f);
	start = std::clock();
	derandomizedSolver.solve();
	double duration2 = (std::clock() - start) / static_cast<double>(CLOCKS_PER_SEC);
	int alg2 = derandomizedSolver.getResult();

	List<double> p_list;

	using namespace szx;
	std::cout << "=== 3. LP Solver ===" << std::endl;
	LPSolver lpSolver(f);
	start = std::clock();
	lpSolver.solve(p_list);
	double duration3 = (std::clock() - start) / static_cast<double>(CLOCKS_PER_SEC);
	int alg3 = lpSolver.getResult();

	std::cout << "=== 4. LPDerandomized Solver ===" << std::endl;
	LPDerandomizedSolver lpDerandomizedSolver(f);
	start = std::clock();
	lpDerandomizedSolver.solve(p_list);
	double duration4 = (std::clock() - start) / static_cast<double>(CLOCKS_PER_SEC);
	int alg4 = lpDerandomizedSolver.getResult();

	// record solution
	std::ofstream logFile(Cfg::SolutionFolder + year + ".log.csv", std::ios::app);
	std::ostringstream log;
	log << inst_reader.inst_type << "," << inst_reader.inst_name << ","
		<< alg1 << "," << duration1 << "," << alg2 << "," << duration2 << "," << alg3 << "," << duration3 << "," << alg4 << "," << duration4
		<< std::endl;

	logFile.seekp(0, std::ios::end);
	if (logFile.tellp() <= 0) {
		logFile << "Family,Instance,"
				<< "Alg-1,Duration-1,Alg-2,Duration-2,Alg-3,Duration-3,Alg-4,Duration-4" 
				<< std::endl;
	}
	logFile << log.str();
	logFile.close();
}

int main() {
	//test();

#pragma region runSingleInstance
    String year = "mse18";
    String type = "bcp-fir";
    String inst = "bcp-fir-normalized-f20c10b_006_area_delay.wcnf";

//	String year = "mse19";
//	String type = "bcp";
//	String inst = "c5315_F1@0.wcnf";

	runSingleInstance(year, type, inst);
#pragma endregion runSingleInstance


#if _OS_MS_WINDOWS
#include "fileFilter.hpp"

#pragma region runAllInstances
//	String year = "mse19";
//	HashMap<String, List<String>> inst_map;
//	typeFilter(inst_map, year);
//	for (const auto & each : inst_map) {
//		String type = each.first;
//		for (const auto & inst : each.second) {
//			runSingleInstance(year, type, inst);
//		}
//	}
#pragma endregion runAllInstances
	system("PAUSE");
#endif

	return 0;
}