////////////////////////////////
/// usage : 1.	switches for debug log.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SMART_SZX_P_CENTER_LOG_SWITCH_H
#define SMART_SZX_P_CENTER_LOG_SWITCH_H


#include "Utility.h"


namespace szx {

struct LogSwitch {
    // TODO[szx][0]: turn off all logs before the release.
    enum Szx {
        Main = Log::Level::Info,
        Cli = Log::Level::Off,
        Framework = Log::Level::Debug,
        Input = Log::Level::On,
        Output = Log::Level::On,
        Preprocess = Log::Level::On,
        Postprocess = Log::Level::On,
        Config = Log::Level::On,
        Model = Log::Level::Debug,
        MpSolver = Log::Level::On,
        Checker = Log::Level::On,
    };
};

}


#endif // SMART_SZX_P_CENTER_LOG_SWITCH_H
