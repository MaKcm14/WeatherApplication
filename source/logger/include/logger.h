#ifndef LOGGER_HEADER

#    include <fstream>
#    include <mutex>
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
    std::mutex LogMut;

public:
    TLogger();

    ~TLogger() {
        std::lock_guard<std::mutex> logLock(LogMut);
        LogOut.close();
    }


    template <typename TType>
    friend TLogger& operator << (TLogger& logger, const TType& data);

    friend TLogger& operator << (TLogger& logger, const TLogger::ELevel& lev);
    
};


using TLevel = TLogger::ELevel;

template <typename TType>
TLogger& operator << (TLogger& logger, const TType& data) {
    std::lock_guard<std::mutex> logLock(logger.LogMut);
    logger.LogOut << data;
    return logger;
}


#endif


#define LOGGER_HEADER
