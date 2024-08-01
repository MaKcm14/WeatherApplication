#ifndef CACHE_MANAGER_HEADER

#    include <ctime>
#    include "data_base.h"
#    include <iostream>
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
        struct TCityNote {
            std::string City;
            std::string CacheWeatherDesc;
            std::string ExpiredTime;
            bool CheckExistingFlag = false;

            void Clear() noexcept {
                City.clear();
                CacheWeatherDesc.clear();
                ExpiredTime.clear();
                CheckExistingFlag = false;
            }
        };

        std::unique_ptr<NDataBase::TDataBase> DataBase;
        TDataChecker Checker;
        TCityNote CacheNote;

    public:
        TCacheManager(std::unique_ptr<NDataBase::TDataBase> dataBase);
        

        bool IsDataExpired(const std::string& city);

        void InsertOrUpdateData(const std::string& city, const std::string& weathDesc);

        std::string GetData(const std::string& city);
    

    private:
        std::string GetCurTimeWithShift(int32_t shift) const;

        void InsertCache(const std::string& city, const std::string& weathDesc);

        void UpdateCache(const std::string& city, const std::string& weathDesc);

    };

}


#endif


#define CACHE_MANAGER_HEADER
