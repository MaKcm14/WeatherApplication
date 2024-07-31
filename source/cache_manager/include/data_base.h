#ifndef DATA_BASE_HEADER

#    include "data_base_exception.h"
#    include <memory>
#    include <nlohmann/json.hpp>
#    include "postgresql/libpq-fe.h"
#    include <vector>

namespace NDataBase {
    /// @brief abstract class for every RELATIONAL DB
    class TDataBase {
    public:
        virtual std::vector<std::vector<std::string>> Select(const std::string& query) = 0;

        virtual void Insert(const std::string& query) = 0;

        virtual void Update(const std::string& query) = 0;

        virtual void Delete(const std::string& query) = 0;

        virtual void CreateTable(const std::string& query) = 0;

    };


    /// @brief main class for PostgreSql DB
    class TPostgreSql : public TDataBase {
    public:
        TPostgreSql(const std::string& ip, const std::string& port, const std::string& password);

        TPostgreSql(const TPostgreSql& postgres) = delete;

        TPostgreSql(TPostgreSql&& postgres) {
            Connection = postgres.Connection;
            postgres.Connection = nullptr;
        }
        
        ~TPostgreSql() {
            PQfinish(Connection);
        }

        TPostgreSql& operator = (const TPostgreSql& postgres) = delete;

        TPostgreSql& operator = (TPostgreSql&& postgres) {
            Connection = postgres.Connection;
            postgres.Connection = nullptr;
            return *this;
        }

        std::vector<std::vector<std::string>> Select(const std::string& query) override;

        void Insert(const std::string& query) override;

        void Update(const std::string& query) override;

        void Delete(const std::string& query) override;

        void CreateTable(const std::string& query) override;

    private:
        PGconn* Connection;

    };
}

#endif

#define DATA_BASE_HEADER
