// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "include/logger.h"

TLogger::TLogger() 
    : LogOut("log.txt")
{
}


TLogger& operator << (TLogger& logger, const TLogger::ELevel& lev) {
    std::lock_guard<std::mutex> logLock(logger.LogMut);

    logger.LogOut << "# id: " << std::this_thread::get_id() << ":\t";
    if (lev == TLevel::Debug) {
        logger.LogOut << "[Debug]: ";

    } else if (lev == TLevel::Info) {
        logger.LogOut << "[Info]: ";

    } else if (lev == TLevel::Warning) {
        logger.LogOut << "[Warning]: ";

    } else if (lev == TLevel::Error) {
        logger.LogOut << "[Error]: ";
        
    } else if (lev == TLevel::Fatal) {
        logger.LogOut << "[Fatal]: ";
    }
    
    return logger;
}
