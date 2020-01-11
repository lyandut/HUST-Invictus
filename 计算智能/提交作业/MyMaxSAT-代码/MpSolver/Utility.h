////////////////////////////////
/// usage : 1.	utilities.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SMART_SZX_INVENTORY_ROUTING_UTILITY_H
#define SMART_SZX_INVENTORY_ROUTING_UTILITY_H


#include "Config.h"

#include <algorithm>
#include <chrono>
#include <initializer_list>
#include <vector>
#include <map>
#include <set>
#include <random>
#include <iostream>
#include <iomanip>

#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>


#define UTILITY_NOT_IMPLEMENTED  throw "Not implemented yet!";


// [on] use chrono instead of ctime in Timer.
#define UTILITY_TIMER_CPP_STYLE  1

// [off] use chrono instead of ctime in DateTime.
#define UTILITY_DATE_TIME_CPP_STYLE  0


namespace szx {

// if there is "#define x  y", VERBATIM_STRINGIFY(x) will get "x".
#define VERBATIM_STRINGIFY(x)  #x
// if there is "#define x  y", RESOLVED_STRINGIFY(x) will get "y".
#define RESOLVED_STRINGIFY(x)  VERBATIM_STRINGIFY(x)

#define VERBATIM_CONCAT(a, b)  a##b
#define VERBATIM_CONCAT2(a, b, c)  a##b##c
#define VERBATIM_CONCAT3(a, b, c, d)  a##b##c##d
#define RESOLVED_CONCAT(a, b)  VERBATIM_CONCAT(a, b)
#define RESOLVED_CONCAT2(a, b, c)  VERBATIM_CONCAT2(a, b, c)
#define RESOLVED_CONCAT3(a, b, c, d)  VERBATIM_CONCAT3(a, b, c, d)


#ifndef SMART_SZX_CPP_UTILIBS_ARR
#define SMART_SZX_CPP_UTILIBS_ARR
template<typename T, typename IndexType = int>
class Arr {
public:
    // it is always valid before copy assignment due to no reallocation.
    using Iterator = T*;
    using ConstIterator = T const *;

    enum ResetOption { AllBits0 = 0, AllBits1 = -1 };

    explicit Arr() : arr(nullptr), len(0) {}
    explicit Arr(IndexType length) { allocate(length); }
    explicit Arr(IndexType length, T *data) : arr(data), len(length) {}
    explicit Arr(IndexType length, const T &defaultValue) : Arr(length) {
        std::fill(arr, arr + length, defaultValue);
    }
    explicit Arr(std::initializer_list<T> l) : Arr(static_cast<IndexType>(l.size())) {
        std::copy(l.begin(), l.end(), arr);
    }

    Arr(const Arr &a) : Arr(a.len) {
        if (this != &a) { copyData(a.arr); }
    }
    Arr(Arr &&a) : Arr(a.len, a.arr) { a.arr = nullptr; }

    Arr& operator=(const Arr &a) {
        if (this != &a) {
            if (len != a.len) {
                clear();
                init(a.len);
            }
            copyData(a.arr);
        }
        return *this;
    }
    Arr& operator=(Arr &&a) {
        if (this != &a) {
            delete[] arr;
            arr = a.arr;
            len = a.len;
            a.arr = nullptr;
        }
        return *this;
    }

    ~Arr() { clear(); }

    // allocate memory if it has not been init before.
    bool init(IndexType length) {
        if (arr == nullptr) { // avoid re-init and memory leak.
            allocate(length);
            return true;
        }
        return false;
    }

    // remove all items.
    void clear() {
        delete[] arr;
        arr = nullptr;
    }

    // set all data to val. any value other than 0 or -1 is undefined behavior.
    void reset(ResetOption val = ResetOption::AllBits0) { memset(arr, val, sizeof(T) * len); }

    T& operator[](IndexType i) { return arr[i]; }
    const T& operator[](IndexType i) const { return arr[i]; }

    T& at(IndexType i) { return arr[i]; }
    const T& at(IndexType i) const { return arr[i]; }

    Iterator begin() { return arr; }
    Iterator end() { return (arr + len); }
    ConstIterator begin() const { return arr; }
    ConstIterator end() const { return (arr + len); }
    T& front() { return at(0); }
    T& back() { return at(len - 1); }
    const T& front() const { return at(0); }
    const T& back() const { return at(len - 1); }

    IndexType size() const { return len; }
    bool empty() const { return (len == 0); }

protected:
    // must not be called except init.
    void allocate(IndexType length) {
        // TODO[szx][2]: length > (1 << 32)?
        arr = new T[static_cast<size_t>(length)];
        len = length;
    }

    void copyData(T *data) {
        // TODO[szx][1]: what if data is shorter than arr?
        // OPTIMIZE[szx][8]: use memcpy() if all callers are POD type.
        std::copy(data, data + len, arr);
    }


    T *arr;
    IndexType len;
};

template<typename T, typename IndexType = int>
class Arr2D {
public:
    // it is always valid before copy assignment due to no reallocation.
    using Iterator = T*;
    using ConstIterator = T const *;

    enum ResetOption { AllBits0 = 0, AllBits1 = -1 };

    explicit Arr2D() : arr(nullptr), len1(0), len2(0), len(0) {}
    explicit Arr2D(IndexType length1, IndexType length2) { allocate(length1, length2); }
    explicit Arr2D(IndexType length1, IndexType length2, T *data)
        : arr(data), len1(length1), len2(length2), len(length1 * length2) {}
    explicit Arr2D(IndexType length1, IndexType length2, const T &defaultValue) : Arr2D(length1, length2) {
        std::fill(arr, arr + len, defaultValue);
    }

    Arr2D(const Arr2D &a) : Arr2D(a.len1, a.len2) {
        if (this != &a) { copyData(a.arr); }
    }
    Arr2D(Arr2D &&a) : Arr2D(a.len1, a.len2, a.arr) { a.arr = nullptr; }

    Arr2D& operator=(const Arr2D &a) {
        if (this != &a) {
            if (len != a.len) {
                clear();
                init(a.len1, a.len2);
            } else {
                len1 = a.len1;
                len2 = a.len2;
            }
            copyData(a.arr);
        }
        return *this;
    }
    Arr2D& operator=(Arr2D &&a) {
        if (this != &a) {
            delete[] arr;
            arr = a.arr;
            len1 = a.len1;
            len2 = a.len2;
            len = a.len;
            a.arr = nullptr;
        }
        return *this;
    }

    ~Arr2D() { clear(); }

    // allocate memory if it has not been init before.
    bool init(IndexType length1, IndexType length2) {
        if (arr == nullptr) { // avoid re-init and memory leak.
            allocate(length1, length2);
            return true;
        }
        return false;
    }

    // remove all items.
    void clear() {
        delete[] arr;
        arr = nullptr;
    }

    // set all data to val. any value other than 0 or -1 is undefined behavior.
    void reset(ResetOption val = ResetOption::AllBits0) { memset(arr, val, sizeof(T) * len); }

    IndexType getFlatIndex(IndexType i1, IndexType i2) const { return (i1 * len2 + i2); }

    T* operator[](IndexType i1) { return (arr + i1 * len2); }
    const T* operator[](IndexType i1) const { return (arr + i1 * len2); }

    T& at(IndexType i) { return arr[i]; }
    const T& at(IndexType i) const { return arr[i]; }
    T& at(IndexType i1, IndexType i2) { return arr[i1 * len2 + i2]; }
    const T& at(IndexType i1, IndexType i2) const { return arr[i1 * len2 + i2]; }

    Iterator begin() { return arr; }
    Iterator begin(IndexType i1) { return arr + (i1 * len2); }
    ConstIterator begin() const { return arr; }
    ConstIterator begin(IndexType i1) const { return arr + (i1 * len2); }

    Iterator end() { return (arr + len); }
    Iterator end(IndexType i1) { return arr + (i1 * len2) + len2; }
    ConstIterator end() const { return (arr + len); }
    ConstIterator end(IndexType i1) const { return arr + (i1 * len2) + len2; }

    T& front() { return at(0); }
    T& front(IndexType i1) { return at(i1, 0); }
    const T& front() const { return at(0); }
    const T& front(IndexType i1) const { return at(i1, 0); }

    T& back() { return at(len - 1); }
    T& back(IndexType i1) { return at(i1, len - 1); }
    const T& back() const { return at(len - 1); }
    const T& back(IndexType i1) const { return at(i1, len - 1); }

    IndexType size1() const { return len1; }
    IndexType size2() const { return len2; }
    IndexType size() const { return len; }
    bool empty() const { return (len == 0); }

protected:
    // must not be called except init.
    void allocate(IndexType length1, IndexType length2) {
        len1 = length1;
        len2 = length2;
        len = length1 * length2;
        arr = new T[static_cast<size_t>(len)];
    }

    void copyData(T *data) {
        // TODO[szx][1]: what if data is shorter than arr?
        // OPTIMIZE[szx][8]: use memcpy() if all callers are POD type.
        std::copy(data, data + len, arr);
    }


    T *arr;
    IndexType len1;
    IndexType len2;
    IndexType len;
};
#endif // !SMART_SZX_CPP_UTILIBS_ARR


class Random {
public:
    using Generator = std::mt19937;


    Random(int seed) : rgen(seed) {}
    Random() : rgen(generateSeed()) {}


    static int generateSeed() {
        return static_cast<int>(std::time(nullptr) + std::clock());
    }

    Generator::result_type operator()() { return rgen(); }

    // pick with probability of (numerator / denominator).
    bool isPicked(unsigned numerator, unsigned denominator) {
        return ((rgen() % denominator) < numerator);
    }

    // pick from [min, max).
    int pick(int min, int max) {
        return ((rgen() % (max - min)) + min);
    }
    // pick from [0, max).
    int pick(int max) {
        return (rgen() % max);
    }


    Generator rgen;
};

// count | 1 2 3 4 ...  k   k+1   k+2   k+3  ...  n
// ------|------------------------------------------
// index | 0 1 2 3 ... k-1   k    k+1   k+2  ... n-1
// prob. | 1 1 1 1 ...  1  k/k+1 k/k+2 k/k+3 ... k/n
class Sampling {
public:
    Sampling(Random &randomNumberGenerator, int targetNumber)
        : rgen(randomNumberGenerator), targetNum(targetNumber), pickCount(0) {}

    // return 0 for not picked.
    // return an integer i \in [1, targetNum] if it is the i_th item in the picked set.
    int isPicked() {
        if ((++pickCount) <= targetNum) {
            return pickCount;
        } else {
            int i = rgen.pick(pickCount) + 1;
            return (i <= targetNum) ? i : 0;
        }
    }

    // return -1 for no need to replace any item.
    // return an integer i \in [0, targetNum) as the index to be replaced in the picked set.
    int replaceIndex() {
        if (pickCount < targetNum) {
            return pickCount++;
        } else {
            int i = rgen.pick(++pickCount);
            return (i < targetNum) ? i : -1;
        }
    }

    void reset() {
        pickCount = 0;
    }

protected:
    Random &rgen;
    int targetNum;
    int pickCount;
};


class Timer {
public:
    #if UTILITY_TIMER_CPP_STYLE
    using Millisecond = std::chrono::milliseconds;
    using TimePoint = std::chrono::steady_clock::time_point;
    using Clock = std::chrono::steady_clock;
    #else
    using Millisecond = int;
    using TimePoint = int;
    struct Clock {
        static TimePoint now() { return clock(); }
    };
    #endif // UTILITY_TIMER_CPP_STYLE


    static constexpr double MillisecondsPerSecond = 1000;
    static constexpr double ClocksPerSecond = CLOCKS_PER_SEC;
    static constexpr int ClocksPerMillisecond = static_cast<int>(ClocksPerSecond / MillisecondsPerSecond);


    #if UTILITY_TIMER_CPP_STYLE
    Timer(const Millisecond &duration, const TimePoint &st = Clock::now())
        : startTime(st), endTime(startTime + duration) {}
    #else
    Timer(const Millisecond &duration, const TimePoint &st = Clock::now())
        : startTime(st), endTime(startTime + duration * ClocksPerMillisecond) {}
    #endif // UTILITY_TIMER_CPP_STYLE

    static Millisecond durationInMillisecond(const TimePoint &start, const TimePoint &end) {
        #if UTILITY_TIMER_CPP_STYLE
        return std::chrono::duration_cast<Millisecond>(end - start);
        #else
        return (end - start) / ClocksPerMillisecond;
        #endif // UTILITY_TIMER_CPP_STYLE
    }

    static double durationInSecond(const TimePoint &start, const TimePoint &end) {
        #if UTILITY_TIMER_CPP_STYLE
        return std::chrono::duration_cast<Millisecond>(end - start).count() / MillisecondsPerSecond;
        #else
        return (end - start) / ClocksPerSecond;
        #endif // UTILITY_TIMER_CPP_STYLE
    }

    static Millisecond toMillisecond(double second) {
        #if UTILITY_TIMER_CPP_STYLE
        return Millisecond(static_cast<long long>(second * MillisecondsPerSecond));
        #else
        return static_cast<Millisecond>(second * MillisecondsPerSecond);
        #endif // UTILITY_TIMER_CPP_STYLE
    }

    // there is no need to free the pointer. the format of the format string is 
    // the same as std::strftime() in http://en.cppreference.com/w/cpp/chrono/c/strftime.
    static const char* getLocalTime(const char *format = "%Y-%m-%d(%a)%H:%M:%S") {
        static constexpr int DateBufSize = 64;
        static char buf[DateBufSize];
        time_t t = time(NULL);
        tm *date = localtime(&t);
        strftime(buf, DateBufSize, format, date);
        return buf;
    }
    static const char* getTightLocalTime() { return getLocalTime("%Y%m%d%H%M%S"); }

    bool isTimeOut() const {
        return (Clock::now() > endTime);
    }

    Millisecond restMilliseconds() const {
        return durationInMillisecond(Clock::now(), endTime);
    }

    double restSeconds() const {
        return durationInSecond(Clock::now(), endTime);
    }

    Millisecond elapsedMilliseconds() const {
        return durationInMillisecond(startTime, Clock::now());
    }

    double elapsedSeconds() const {
        return durationInSecond(startTime, Clock::now());
    }

    const TimePoint& getStartTime() const { return startTime; }
    const TimePoint& getEndTime() const { return endTime; }

protected:
    TimePoint startTime;
    TimePoint endTime;
};


class DateTime {
public:
    static constexpr int MinutesPerDay = 60 * 24;
    static constexpr int MinutesPerHour = 60;
    static constexpr int SecondsPerMinute = 60;


    // TODO[szx][8]: use different names for the arguments.
    DateTime(int year = 0, int month = 0, int day = 0, int hour = 0, int minute = 0, int second = 0)
        : year(year), month(month), day(day), hour(hour), minute(minute), second(second) {}
    DateTime(tm &t) : DateTime(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min) {}
    DateTime(time_t t) : DateTime(*std::localtime(&t)) {}

    // get an inconsistent tm struct which requires std::mktime() to revise.
    operator std::tm() const {
        std::tm datetime;

        datetime.tm_year = year - 1900;
        datetime.tm_mon = month - 1;
        datetime.tm_mday = day;
        datetime.tm_hour = hour;
        datetime.tm_min = minute;
        datetime.tm_sec = second;

        datetime.tm_isdst = -1;

        //datetime.tm_wday = 0; // ignored by mktime().
        //datetime.tm_yday = 0; // ignored by mktime().

        return datetime;
    }

    operator time_t() const {
        std::tm t = static_cast<std::tm>(*this);
        return std::mktime(&t);
    }

    friend DateTime operator+(const DateTime &dateTime, time_t second) {
        time_t t = static_cast<time_t>(dateTime);
        t += second;
        return DateTime(t);
    }
    friend DateTime operator-(const DateTime &dateTime, time_t second) { return (dateTime + (-second)); }

    friend std::ostream& operator<<(std::ostream &os, DateTime &dateTime) {
        os << dateTime.year << '-'
            << std::setw(2) << std::setfill('0') << dateTime.month << '-'
            << std::setw(2) << std::setfill('0') << dateTime.day << ' '
            << std::setw(2) << std::setfill('0') << dateTime.hour << ':'
            << std::setw(2) << std::setfill('0') << dateTime.minute << ':'
            << std::setw(2) << std::setfill('0') << dateTime.second;
    }

    static double durationInSecond(const DateTime &l, const DateTime &r) {
        #if UTILITY_DATE_TIME_CPP_STYLE
        using Clock = std::chrono::system_clock;
        using TimePoint = Clock::time_point;
        TimePoint tpl = Clock::from_time_t(static_cast<time_t>(l));
        TimePoint tpr = Clock::from_time_t(static_cast<time_t>(r));
        return std::chrono::duration_cast<std::chrono::seconds>(tpl - tpr).count();
        #else
        return std::difftime(static_cast<time_t>(l), static_cast<time_t>(r));
        #endif // UTILITY_DATE_TIME_CPP_STYLE
    };


    int year; // year since Common Era.
    int month; // months in year.
    int day; // days in month.
    int hour; // hours in day.
    int minute; // minutes in hour.
    int second; // seconds in minute.
};


class Log {
public:
    using Manipulator = std::ostream& (*)(std::ostream&);


    enum Level {
        On,
        Off, // the default state if not specified.

        Fatal = On,
        Error = On,
        Warning = On,
        Debug = On,
        Info, // = Off.
    };


    #if SZX_DEBUG
    static bool isTurnedOn(int level) { return (level == On); }
    static bool isTurnedOff(int level) { return !isTurnedOn(level); }
    #else
    static bool isTurnedOn(int level) { return false; }
    static bool isTurnedOff(int level) { return true; }
    #endif // SZX_DEBUG


    Log(int logLevel, std::ostream &logFile) : level(logLevel), os(logFile) {}
    Log(int logLevel) : Log(logLevel, std::cerr) {}


    template<typename T>
    Log& operator<<(const T &obj) {
        if (isTurnedOn(level)) { os << obj; }
        return *this;
    }
    Log& operator<<(Manipulator manip) {
        if (isTurnedOn(level)) { os << manip; }
        return *this;
    }

protected:
    int level;
    std::ostream &os;
};


template<typename ArbitraryId = int, typename ConsecutiveId = int, const ConsecutiveId DefaultIdNumberHint = 1024>
class ZeroBasedConsecutiveIdMap {
public:
    ZeroBasedConsecutiveIdMap(ConsecutiveId idNumberHint = DefaultIdNumberHint) : count(-1) {
        idList.reserve(static_cast<size_t>(idNumberHint));
    }


    // track a new arbitrary ID or return the sequence of a tracked one.
    ConsecutiveId toConsecutiveId(ArbitraryId arbitraryId) {
        auto iter = idMap.find(arbitraryId);
        if (iter != idMap.end()) { return iter->second; }
        idList.push_back(arbitraryId);
        return (idMap[arbitraryId] = (++count));
    }

    // return the consecutiveId_th tracked arbitrary ID.
    ArbitraryId toArbitraryId(ConsecutiveId consecutiveId) const { return idList[consecutiveId]; }

    bool isConsecutiveIdExist(ConsecutiveId consecutiveId) const { return (consecutiveId <= count); }
    bool isArbitraryIdExist(ArbitraryId arbitraryId) const { return (idMap.find(arbitraryId) != idMap.end()); }


    // number of tracked IDs.
    ConsecutiveId count;
    // idList[consecutiveId] == arbitraryId.
    std::vector<ArbitraryId> idList;
    // idMap[arbitraryId] == consecutiveId.
    std::map<ArbitraryId, ConsecutiveId> idMap;
};


template<typename Unit>
struct Interval {
    Interval() {}
    Interval(Unit intervalBegin, Unit intervalEnd) : begin(intervalBegin), end(intervalEnd) {}

    bool cover(Unit x) const { return ((begin <= x) && (x < end)); }
    bool cover(const Interval &i) const { return ((begin <= i.begin) && (i.end <= end)); }
    bool beginBefore(Unit x) const { return (begin < x); }
    bool beginBefore(const Interval &i) const { return (begin < i.begin); }
    bool endBefore(Unit x) const { return (end <= x); }
    bool endBefore(const Interval &i) const { return (end < i.end); }
    // return true if this is strictly before i (no overlap).
    bool before(const Interval &i) const { return (end <= i.begin); }

    bool isValid() const { return (begin < end); }
    static bool isValid(const Interval &i) { return i.isValid(); }

    static bool isOverlapped(const Interval &l, const Interval &r) {
        return ((l.begin < r.end) && (r.begin < l.end));
    }

    // vector measurement of the interval span.
    Unit displacement() const { return (end - begin); }
    // scalar measurement of the interval span.
    Unit length() const { return std::abs(end - begin); }

    // return the intersection of l and r if they are overlapped,
    // or the reversed gap between them if there is no intersection.
    static Interval overlap(const Interval &l, const Interval &r) {
        return Interval(std::max(l.begin, r.begin), std::min(l.end, r.end));
    }

    // return the length of the blank space between l and r if they are not interseted,
    // or the opposite number of the minimal distance to make them mutually exclusive.
    static Unit gap(const Interval &l, const Interval &r) {
        if (l.begin < r.begin) {
            if (l.end < r.end) {
                return r.begin - l.end;
            } else { // if (l.end >= r.end)
                return std::max(r.begin - l.end, l.begin - r.end);
            }
        } else { // if (l.begin >= r.end)
            if (l.end < r.end) {
                return std::max(r.begin - l.end, l.begin - r.end);
            } else { // if (l.end >= r.end)
                return l.begin - r.end;
            }
        }
    }

    Unit begin;
    Unit end;
};


class System {
public:
    struct Shell {
        struct Common {
            static std::string RedirectStdin() { return " 0< "; }
            static std::string RedirectStdout() { return " 1> "; }
            static std::string RedirectStderr() { return " 2> "; }
            static std::string RedirectStdout_app() { return  " 1>> "; }
            static std::string RedirectStderr_app() { return  " 2>> "; }
        };
        struct Win32 : public Common {
            static std::string Mkdir() { return " mkdir "; }
            static std::string NullDev() { return " nul "; }
        };
        struct Unix : public Common {
            static std::string Mkdir() { return  " mkdir -p "; }
            static std::string NullDev() { return  " /dev/null "; }

        };
    };

    #if _OS_MS_WINDOWS
    using Cmd = Shell::Win32;
    #else
    using Cmd = Shell::Unix;
    #endif // _OS_MS_WINDOWS

    static int exec(const std::string &cmd) { return system(cmd.c_str()); }

    static std::string quote(const std::string &s) { return ('\"' + s + '\"'); }

    static void makeSureDirExist(const std::string &dir) {
        exec(Cmd::Mkdir() + quote(dir) + Cmd::RedirectStderr() + Cmd::NullDev());
    }

    struct MemorySize {
        using Unit = long long;

        static constexpr Unit Base = 1024;

        friend std::ostream& operator<<(std::ostream &os, const MemorySize &memSize) {
            auto units = { "B", "KB", "MB", "GB", "TB", "PB" };
            double size = static_cast<double>(memSize.size);
            for (auto u = units.begin(); u != units.end(); ++u, size /= Base) {
                if (size < Base) {
                    os << std::setprecision(4) << size << *u;
                    break;
                }
            }

            return os;
        }

        Unit size;
    };

    struct MemoryUsage {
        MemorySize physicalMemory;
        MemorySize virtualMemory;
    };

    static MemoryUsage memoryUsage();
    static MemoryUsage peakMemoryUsage();
};


class Math {
public:
    static constexpr double DefaultTolerance = 0.01;

    static bool weakEqual(double l, double r, double tolerance = DefaultTolerance) {
        return (std::abs(l - r) < tolerance);
    }
    static bool weakLess(double l, double r, double tolerance = DefaultTolerance) { // operator<=().
        return ((l - r) < tolerance);
    }
    static bool strongLess(double l, double r, double tolerance = DefaultTolerance) { // operator<().
        return ((l - r) < -tolerance);
    }

    static double floor(double d) { return std::floor(d + DefaultTolerance); }
    static long lfloor(double d) { return static_cast<long>(d + DefaultTolerance); }

    template<typename T>
    static bool isOdd(T integer) { return ((integer % 2) == 1); }
    template<typename T>
    static bool isEven(T integer) { return ((integer % 2) == 0); }

    template<typename T>
    static T bound(T num, T lb, T ub) {
        return std::min(std::max(num, lb), ub);
    }

    template<typename T>
    static bool contains(const std::set<T> &s, const T &value) {
        return s.find(value) != s.end();
    }
};

}


#endif // SMART_SZX_INVENTORY_ROUTING_H
