#ifndef CACHE_MANAGER_HEADER

#    include <ctime>
#    include "logger.h"
#    include <memory>
#    include <nlohmann/json.hpp>
#    include "postgresql/libpq-fe.h"
#    include "request_exception.h"
#    include <sstream>

namespace NRequest {

    extern void ConfigureRequestService();

    /// @brief Describes the connection to the DB
    class TPostgresConnection {
    public:
        TPostgresConnection();

        ~TPostgresConnection() {
            PQfinish(Connection);
        }

        auto GetConnectionStatus() const noexcept;

        PGconn* GetConnection() noexcept {
            return Connection;
        }

    protected:
        PGconn* Connection;

    };


    /// @brief Let make notes (weather description) into the DB and get out the description from DB
    class TCacheManager {
    public:
        TCacheManager();

        bool IsDataExpired(const std::string& city);

        void InsertOrUpdateData(const std::string& city, const std::string& weathDesc);

        std::string GetData(const std::string& city);

    private:
        bool CheckExistingData(const std::string& city);

        bool IsDataSafety(const std::string& city) const;
        
    private:
        TPostgresConnection Connection;

    };

}


#endif

#define CACHE_MANAGER_HEADER
