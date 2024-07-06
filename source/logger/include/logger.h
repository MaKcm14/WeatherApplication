#ifndef LOGGER_HEADER

#    include <fstream>

/// @brief Simple logger class for make notes about events in the programme
class TLogger {
public:
    enum ELevel {
        Debug, // for some base info events
        Info, // for once info events
        Warning, // for uncorrect behaviour of some service that doesn't influence on work globally
        Error, // for errors during the application work that can be fixed
        Fatal // for unpredictable and unfixable errors during the application work (application falls down)
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
