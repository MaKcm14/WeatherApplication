#ifndef LOGGER_HEADER

#    include <fstream>
#    include <thread>

/// @brief simple logger class for make notes about events in the programme
class TLogger {
public:
    enum ELevel {
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

private:
    std::ofstream LogOut;

public:
    TLogger();

    ~TLogger() {
        LogOut.close();
    }


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
