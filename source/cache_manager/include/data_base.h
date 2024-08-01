#ifndef DATA_BASE_HEADER

#    include "data_base_exception.h"
#    include <memory>
#    include "postgresql/libpq-fe.h"
#    include <vector>

namespace NDataBase {

    /// @brief abstract class for every RDB
    class TDataBase {
    public:
        virtual std::vector<std::vector<std::string>> ISelect(const std::string& query) = 0;

        virtual void IInsert(const std::string& query) = 0;

        virtual void IUpdate(const std::string& query) = 0;

        virtual void IDelete(const std::string& query) = 0;

        virtual void ICreateTable(const std::string& query) = 0;

    };


    /// @brief main class for PostgreSql DB
    class TPostgreSql : public TDataBase {
    private:
        PGconn* Connection;

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


        std::vector<std::vector<std::string>> ISelect(const std::string& query) override;

        void IInsert(const std::string& query) override;

        void IUpdate(const std::string& query) override;

        void IDelete(const std::string& query) override;

        void ICreateTable(const std::string& query) override;

    };

}


#endif


#define DATA_BASE_HEADER
