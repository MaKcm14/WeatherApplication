#ifndef DATA_BASE_EXCEPTION_HEADER

#    include <exception>
#    include <string>

namespace NDataBase {

    /// @brief class for the DB's exceptions
    class TDataBaseException : public std::exception {
    private:
        std::string What;

    public:
        TDataBaseException(const std::string& what)
            : What(what)
        {
        }


        const char* what() const noexcept override {
            return What.c_str();
        }

    };

}


#endif


#define DATA_BASE_EXCEPTION_HEADER
