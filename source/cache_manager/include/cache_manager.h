#ifndef CACHE_MANAGER_HEADER

#    include "logger.h"
#    include <memory>
#    include <nlohmann/json.hpp>
#    include "postgresql/libpq-fe.h"
#    include "request_exception.h"

namespace NRequest {

    extern void InitConfig();

    /// @brief Open the connection to the DB
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

    private:
        PGconn* Connection;

    };


    /// @brief Let make notes (weather description) into the DB and get out the description from DB
    class TCacheManager {
    public:
        TCacheManager();
        
    private:
        TPostgresConnection Connection;

    };

}


#endif

#define CACHE_MANAGER_HEADER
