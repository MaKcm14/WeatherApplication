#include "include/logger.h"

TLogger::TLogger() 
    : LogOut("log.txt")
{
}


TLogger& operator << (TLogger& logger, const TLogger::ELevel& lev) {
    if (lev == TLevel::Debug) {
        logger.LogOut << "[Debug]: ";

    } else if (lev == TLevel::Info) {
        logger.LogOut << "[Info]: ";

    } else if (lev == TLevel::Warning) {
        logger.LogOut << "[Warning]: ";

    } else if (lev == TLevel::Error) {
        logger.LogOut << "[Error]: ";
    }
    return logger;
}
