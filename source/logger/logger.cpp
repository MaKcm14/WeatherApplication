#include "include/logger.h"

TLogger::TLogger() 
    : LogOut("log.txt")
{
}


TLogger& operator << (TLogger& logger, const ELevel& lev) {
    if (lev == ELevel::Debug) {
        logger.LogOut << "[Debug]: ";
    } else if (lev == ELevel::Info) {
        logger.LogOut << "[Info]: ";
    } else if (lev == ELevel::Warning) {
        logger.LogOut << "[Warning]: ";
    } else if (lev == ELevel::Error) {
        logger.LogOut << "[Error]: ";
    }
    return logger;
}
