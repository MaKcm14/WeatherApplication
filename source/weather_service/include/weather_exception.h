#ifndef WEATHER_EXCEPTION_HEADER

#    include <exception>
#    include <string>

namespace NWeather {
    
    /// @brief Exceptions specially for weather service actions
    /// ~ error's id classification:
    /// - 400 - client's errors:
    ///        - 401 (error while openning the connection with the resource);
    ///        - 402 (using the wrong HTTP-method);

    class TWeatherException : std::exception {
    public:
        TWeatherException(const std::string& what, int32_t errorId)
        : What(what)
        , ErrorId(errorId)
        {
        }

        const char* what() const noexcept override {
            return What.c_str();
        }

        int32_t GetErrorId() const noexcept {
            return ErrorId;
        }

    private:
        std::string What;
        int32_t ErrorId;

    };

}

#endif

#define WEATHER_EXCEPTIN_HEADER
