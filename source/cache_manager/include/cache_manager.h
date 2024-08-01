#ifndef CACHE_MANAGER_HEADER

#    include <ctime>
#    include "data_base.h"
#    include "logger.h"
#    include <memory>
#    include <nlohmann/json.hpp>
#    include "postgresql/libpq-fe.h"
#    include "request_exception.h"
#    include <sstream>
#    include <vector>

extern TLogger logger;

namespace NRequest {

    extern nlohmann::json configJson;

    extern void ConfigureRequestService();

    /// @brief check the data that was got from the DB (the part of the TCacheManager) 
    class TDataChecker {
    public:
        bool IsDataSafety(const std::string& city) const;

    };


    /// @brief make notes (weather description) in the DB and get out the description from the DB
    class TCacheManager {
    private:
        std::unique_ptr<NDataBase::TDataBase> DataBase;
        TDataChecker Checker;

    public:
        TCacheManager(std::unique_ptr<NDataBase::TDataBase> dataBase);
        

        bool IsDataExpired(const std::string& city);

        void InsertOrUpdateData(const std::string& city, const std::string& weathDesc);

        std::string GetData(const std::string& city);
    

    private:
        bool CheckExistingData(const std::string& city);

        std::string GetCurTimeWithShift(int32_t shift) const;

    };

}


#endif


#define CACHE_MANAGER_HEADER
