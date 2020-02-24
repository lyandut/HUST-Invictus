#ifndef MDK_ATOM_H
#define MDK_ATOM_H

#include "FixLengthInt.h"

#ifdef WIN32
//为了不include <windows.h>
#endif

namespace mdk
{

//自增,返回新值
uint32 AtomSelfAdd(void * var); 

//自减,返回新值
uint32 AtomSelfDec(void * var);

//加一个值,返回旧值
uint32 AtomAdd(void * var, const uint32 value);

//减一个值,返回旧值
uint32 AtomDec(void * var, int32 value);

//赋值,windows下返回新值，linux下返回旧值
uint32 AtomSet(void * var, const uint32 value);

//取值
uint32 AtomGet(void * var);

} //namespace mdk

#endif //MDK_ATOM_H
