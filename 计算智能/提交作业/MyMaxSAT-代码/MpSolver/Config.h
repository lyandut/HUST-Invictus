////////////////////////////////
/// usage : 1.	switches for functionalities.
///         2.	every header file should include this file first (except files in Lib).
/// 
/// note  : 1.	tag macros with [on/off] to indicate prefered state.
////////////////////////////////

#ifndef SMART_SZX_P_CENTER_CONFIG_H
#define SMART_SZX_P_CENTER_CONFIG_H


#pragma region PlatformCheck
#ifdef _MSC_VER // use (_WIN16 || _WIN32 || _WIN64)?
#define _OS_MS_WINDOWS  1
#define _CC_MS_VC  1

#if _MSC_VER >= 1910
#define _CC_VERSION  2017
#elif _MSC_VER >= 1900
#define _CC_VERSION  2015
#elif _MSC_VER >= 1800
#define _CC_VERSION  2013
#elif _MSC_VER >= 1700
#define _CC_VERSION  2012
#elif _MSC_VER >= 1600
#define _CC_VERSION  2010
#elif _MSC_VER >= 1500
#define _CC_VERSION  2008
#elif _MSC_VER >= 1400
#define _CC_VERSION  2005
#elif _MSC_VER
#define _CC_VERSION  2003
#else // _MSC_VER
#define _CC_VERSION  OTHER_COMPILER // error
#endif // _MSC_VER
#else
#define _OS_MS_WINDOWS  0
#define _CC_MS_VC  0
#endif // _MSC_VER

#ifdef __unix__
#define _OS_UNIX  1
#else
#define _OS_UNIX  0
#endif // __unix__

#ifdef __linux__
#define _OS_GNU_LINUX  1
#else
#define _OS_GNU_LINUX  0
#endif // __linux__

#ifdef __MACH__ // use __APPLE__?
#define _OS_APPLE_MAC  1
#else
#define _OS_APPLE_MAC  0
#endif // __MACH__

#ifdef __GNUC__
#define _CC_GNU_GCC  1
#else
#define _CC_GNU_GCC  0
#endif // __GNUC__

#ifdef __llvm__
#define _CC_LLVM  1
#else
#define _CC_LLVM  0
#endif // __llvm__

#ifdef __clang__
#define _CC_CLANG  1
#else
#define _CC_CLANG  0
#endif // __clang__
#pragma endregion PlatformCheck

#pragma region LinkLibraryCheck
#if (_DLL || _SHARED) && !(_STATIC) // prefer static when both are (un)defined.
#define _LL_DYNAMIC  1
#define _LL_STATIC  0
#else
#define _LL_DYNAMIC  0
#define _LL_STATIC  1
#endif // _DLL

#if (_DEBUG || DEBUG) && !(NDEBUG || _NDEBUG || RELEASE || _RELEASE) // prefer release when both are (un)defined.
#define _DR_RELEASE  0
#define _DR_DEBUG  1
#else
#define _DR_RELEASE  1
#define _DR_DEBUG  0
#endif // _DEBUG
#pragma endregion LinkLibraryCheck


#pragma region DebugHelper
// [off] activate test codes like additional check for data consistency..
#define SZX_DEBUG  _CC_MS_VC
#pragma endregion DebugHelper


#pragma region SolverBehavior
#pragma endregion SolverBehavior


#endif // SMART_SZX_P_CENTER_CONFIG_H
