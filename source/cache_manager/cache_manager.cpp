#include "include/cache_manager.h"

extern TLogger logger;

namespace NRequest {
    extern nlohmann::json configJson;
}


NRequest::TPostgresConnection::TPostgresConnection() {
    try {
        std::string password = configJson.at("db_password").dump();
        logger << TLevel::Info << "setting the connection to the DB\n";

        Connection = PQsetdbLogin("127.0.0.1", "5432", nullptr, nullptr, "postgres", "postgres", 
            password.substr(1, password.size() - 2).c_str());

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Fatal << "error while setting the password to the DB: ";
        logger << typeExcp.what() << "\n";
        logger << TLevel::Fatal << "\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Fatal << "error while setting the password to the DB: ";
        logger << parseExcp.what() << "\n";
        logger << TLevel::Fatal << "\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);

    } catch (std::exception& excp) {
        logger << TLevel::Fatal << "error while the setting the connection to the DB: " << excp.what();
        throw TRequestException(excp.what(), 401); 
    }
}


auto NRequest::TPostgresConnection::GetConnectionStatus() const noexcept {
    return PQstatus(Connection);
}


NRequest::TCacheManager::TCacheManager() {
    if (Connection.GetConnectionStatus() == CONNECTION_BAD) {
        logger << TLevel::Fatal << "connection to the DB wasn't set\n";
        logger << PQerrorMessage(Connection.GetConnection()) << "\n\n";
        throw TRequestException("connection to the DB wasn't set correctly", 403);
    }

    logger << TLevel::Info << "connection to the DB was set\n\n";
    logger << TLevel::Info << "checking the existing of the table weather "
        "(and create it if not exists)\n";

    auto checkOrCreate = PQexec(Connection.GetConnection(), "CREATE TABLE IF NOT EXISTS weather ("
    "id SERIAL PRIMARY KEY, city VARCHAR(40), description VARCHAR(300), expires TIMESTAMP)");
    
    if (PQresultStatus(checkOrCreate) == PGRES_FATAL_ERROR ||
        PQresultStatus(checkOrCreate) == PGRES_NONFATAL_ERROR) 
    {
        PQclear(checkOrCreate);

        logger << TLevel::Fatal << "the query of creating/checking the relation ";
        logger << "wasn't processed coorectly\n";
        logger << PQerrorMessage(Connection.GetConnection()) << "\n\n";

        throw TRequestException("the query of creating/checking the relation "
            "wasn't processed coorectly", 504);
    }

    logger << TLevel::Info << "the relation was created/checked correctly\n\n";
    PQclear(checkOrCreate);
}


/// @return Return false if data aren't in the DB or it has the expired time status
/// if data hasn't expired time status function returns true 
bool NRequest::TCacheManager::CheckData(const std::string& city) {
    if (city.empty()) {
        logger << TLevel::Warning << "the city name isn't correct (empty)\n\n";
        throw TRequestException("the city name isn't correct (empty)", 401);
    }

    /// TODO: check the SQL-injection
    logger << TLevel::Debug << "checking the data are in the DB and hasn't expired time status\n";

    time_t sysCurrentTime = time(NULL);
    tm* currentUTCTime = gmtime(&sysCurrentTime);
    std::ostringstream timeUTCStream;

    timeUTCStream << (currentUTCTime->tm_year + 1900) << "-";
    timeUTCStream << (currentUTCTime->tm_mon + 1) << "-";
    timeUTCStream << currentUTCTime->tm_mday << " ";
    timeUTCStream << currentUTCTime->tm_hour << ":";
    timeUTCStream << currentUTCTime->tm_min << ":00'";

    auto checkResult = PQexec(Connection.GetConnection(), 
        (std::string("SELECT * FROM weather\r\nWHERE city = '") + city +
         std::string("' AND expires >= '") + timeUTCStream.str()).c_str());

    if (PQresultStatus(checkResult) == PGRES_FATAL_ERROR || 
        PQresultStatus(checkResult) == PGRES_NONFATAL_ERROR)
    {
        PQclear(checkResult);

        logger << TLevel::Error << "the query of the checking the city note ";
        logger << "wasn't processed coorectly\n";
        logger << PQerrorMessage(Connection.GetConnection()) << "\n\n";

        throw TRequestException("the query of the checking the city note wasn't processed coorectly", 402);
    }
    
    logger << TLevel::Debug << "the query of the checking the city note finished correctly\n";
    if (!PQntuples(checkResult)) {
        PQclear(checkResult);
        logger << TLevel::Debug << "the note for the city '" << city << "' wasn't found\n\n";
        return false;
    }

    PQclear(checkResult);

    logger << TLevel::Debug << "the note for the city '" << city << "' was found correctly\n\n";
    return true;
}