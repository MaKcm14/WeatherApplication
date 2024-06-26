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

    TLogger() 
    : LogOut("log.txt")
    {
    }

    template <class TType>
    friend TLogger& operator << (TLogger& logger, const TType& data);

    friend TLogger& operator << (TLogger& logger, const ELevel& lev);

private:
    std::ofstream LogOut;
};

#endif

#define LOGGER_HEADER
