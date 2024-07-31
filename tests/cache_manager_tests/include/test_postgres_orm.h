#ifndef TEST_POSTGRES_ORM

#    include "cache_manager.h"
#    include <ctime>
#    include <sstream>
#    include <vector>

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


class TTestPostgresORM : private TPostgresConnection {
public:
    std::vector<std::string> Select(const std::string& query);

    void Insert(const std::string& query);

    void Update(const std::string& query);

    void Delete(const std::string& query);

    /// @param shift in the mins
    std::string GetCurTimeWithShift(int32_t shift);

};


#endif

#define TEST_POSTGRES_ORM