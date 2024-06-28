#ifndef REQUEST_EXCEPTION_HEADER

#    include <exception>
#    include <string>

namespace NRequest {

    /// @brief Exceptions specially for request actions
    class TRException : public std::exception {
    public:
        TRException(const std::string& description)
            : What(description)
        {
        }

        const char* what() const noexcept override {
            return What.c_str();
        }

    private:
        std::string What;

    };


}


#endif

#define REQUEST_EXCEPTION_HEADER
