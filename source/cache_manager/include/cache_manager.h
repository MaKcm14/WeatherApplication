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

    extern void ConfigureRequestService();

    extern nlohmann::json configJson;

    class TDataChecker {
    public:
        bool IsDataSafety(const std::string& city) const;

    };

    /// @brief Let make notes (weather description) into the DB and get out the description from DB
    class TCacheManager {
    public:
        TCacheManager(std::unique_ptr<NDataBase::TDataBase> dataBase);

        bool IsDataExpired(const std::string& city);

        void InsertOrUpdateData(const std::string& city, const std::string& weathDesc);

        std::string GetData(const std::string& city);

        bool CheckExistingData(const std::string& city);

    private:
        std::unique_ptr<NDataBase::TDataBase> DataBase;
        TDataChecker Checker;

    };

}


#endif

#define CACHE_MANAGER_HEADER
