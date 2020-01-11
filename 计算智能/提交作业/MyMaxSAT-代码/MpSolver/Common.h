////////////////////////////////
/// usage : 1.	common type aliases.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SMART_SZX_P_CENTER_COMMON_H
#define SMART_SZX_P_CENTER_COMMON_H


#include <vector>
#include <set>
#include <map>
#include <string>


namespace szx {

// zero-based consecutive integer identifier.
using ID = int;
// the unit of width and height.
using Length = int;
// the unit of x and y coordinates.
using Coord = Length;
// the unit of elapsed computational time.
using Duration = int;
// number of neighborhood moves in local search.
using Iteration = int;

template<typename T>
using List = std::vector<T>;

template<typename T>
using Set = std::set<T>;

template<typename Key, typename Val>
using Map = std::map<Key, Val>;

using String = std::string;


class FileExtension {
public:
    static String protobuf() { return String(".pb"); }
    static String json() { return String(".json"); }
};

}


#endif // SMART_SZX_P_CENTER_COMMON_H
