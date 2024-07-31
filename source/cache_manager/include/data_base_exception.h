#ifndef DATA_BASE_EXCEPTION_HEADER

#    include <exception>
#    include <string>

namespace NDataBase {

    class TDataBaseException : public std::exception {
    public:
        TDataBaseException(const std::string& what)
            : What(what)
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

#define DATA_BASE_EXCEPTION_HEADER
