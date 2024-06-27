#ifndef LOGGER_HEADER

#    include <fstream>

 enum class ELevel {
    Debug,
    Info,
    Warning,
    Error
};


class TLogger {
public:

    TLogger();

    template <class TType>
    friend TLogger& operator << (TLogger& logger, const TType& data);

    friend TLogger& operator << (TLogger& logger, const ELevel& lev);

private:
    std::ofstream LogOut;
};


template <class TType>
TLogger& operator << (TLogger& logger, const TType& data) {
    logger.LogOut << data;

    return logger;
}


#endif

#define LOGGER_HEADER
