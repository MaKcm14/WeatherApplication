#ifndef LOGGER_HEADER

#    include <fstream>

/// @brief Simple logger class for make notes about events in the programme
class TLogger {
public:
    enum ELevel {
        Debug,
        Info,
        Warning,
        Error
    };

private:
    std::ofstream LogOut;

public:
    TLogger();

    template <typename TType>
    friend TLogger& operator << (TLogger& logger, const TType& data);

    friend TLogger& operator << (TLogger& logger, const TLogger::ELevel& lev);
    
};


using TLevel = TLogger::ELevel;

template <typename TType>
TLogger& operator << (TLogger& logger, const TType& data) {
    logger.LogOut << data;
    
    return logger;
}


#endif

#define LOGGER_HEADER
