//
// Created by liyan on 2019/10/27.
//

#ifndef MYMAXSAT_FILEFILTER_HPP
#define MYMAXSAT_FILEFILTER_HPP

#include <io.h>
#include "config.h"

static void instFilter(List<String> &inst_list, String year, String type) {
	String filter = Cfg::InstanceFolder + year + '/' + type + "/*.*cnf";
	_finddata_t inst_data;
	auto handler = _findfirst(filter.c_str(), &inst_data);
	if (handler == -1) {
		std::cout << filter << " not found!" << std::endl;
		return; // [todo] add exception
	}
	do {
		if (strcmp(inst_data.name, ".") == 0 || strcmp(inst_data.name, "..") == 0) { continue; }
		inst_list.emplace_back(inst_data.name);
	} while (_findnext(handler, &inst_data) == 0);
	_findclose(handler);
}

static void typeFilter(HashMap<String, List<String>> &inst_map, String year) {
	String filter = Cfg::InstanceFolder + year + "/*.*";
	_finddata_t type_data;
	auto handler = _findfirst(filter.c_str(), &type_data);
	if (handler == -1) {
		std::cout << filter << " not found!" << std::endl;
		return; // [todo] add exception
	}
	do {
		if (strcmp(type_data.name, ".") == 0 || strcmp(type_data.name, "..") == 0) { continue; }
		if (type_data.attrib & _A_SUBDIR) {
			List<String> inst_list; inst_list.reserve(40);
			instFilter(inst_list, year, type_data.name);
			inst_map[type_data.name] = inst_list;
		}
	} while (_findnext(handler, &type_data) == 0);
	_findclose(handler);
}


#endif // MYMAXSAT_FILEFILTER_HPP