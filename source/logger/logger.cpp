#ifndef LOGGER_SOURCE

#    include "include/logger.h"

template <class TType>
TLogger& operator << (TLogger& logger, const TType& data) {
    logger.LogOut << data;

    return logger;
}

TLogger& operator << (TLogger& logger, const ELevel& lev) {
    if (lev == ELevel::Debug) {
        logger.LogOut << "L: 'Debug': ";
    } else if (lev == ELevel::Info) {
        logger.LogOut << "L: 'Info': ";
    } else if (lev == ELevel::Warning) {
        logger.LogOut << "L: 'Warning': ";
    } else if (lev == ELevel::Error) {
        logger.LogOut << "L: 'Error': ";
    }
    return logger;
}

#endif

#define LOGGER_SOURCE
