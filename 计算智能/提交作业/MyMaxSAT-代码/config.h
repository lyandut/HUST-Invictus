//
// Created by 李研 on 2019/10/22.
//

#ifndef MYMAXSAT_CONFIG_H
#define MYMAXSAT_CONFIG_H

#include <vector>
#include <string>
#include <unordered_map>

using ID = int;

using String = std::string;

template <typename T>
using List = std::vector<T>;

template <typename Key, typename Value>
using HashMap = std::unordered_map<Key, Value>;

#define THREAD_ON 1
#define MP_MODEL 1

struct Cfg {
    static constexpr char* InstanceFolder = "../Instance/";
	static constexpr char* SolutionFolder = "../Solution/";
	static constexpr int SoftClauseWeight = 1;
	static constexpr int GRBTimeoutInSec = 1800;
	static constexpr int DerandomizedTimeoutInSec = 1800;
	static constexpr int ThreadNumber = 4;
};


#endif //MYMAXSAT_CONFIG_H
