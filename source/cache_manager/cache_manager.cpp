#include "include/cache_manager.h"

extern TLogger logger;

namespace NRequest {
    extern nlohmann::json configJson;
}


NRequest::TPostgresConnection::TPostgresConnection() {
    logger << TLevel::Info << "getting the password from the DB begun\n";

    try {
        std::string password = configJson.at("db_password").dump();

        logger << TLevel::Info << "the password was got correctly\n";
        logger << TLevel::Info << "setting the connection to the DB\n";

        Connection = PQsetdbLogin("127.0.0.1", "5432", nullptr, nullptr, "postgres", "postgres", 
            password.substr(1, password.size() - 2).c_str());

        if (PQstatus(Connection) == CONNECTION_BAD) {
            logger << TLevel::Fatal << "connection to the DB wasn't set\n";
            logger << PQerrorMessage(Connection) << "\n\n";
            throw TRequestException("connection to the DB wasn't set correctly", 403);
        }  

        logger << TLevel::Info << "connection to the DB was set\n\n";

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Fatal << "error while getting the password to the DB: " << typeExcp.what() << "\n";
        logger << "\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Fatal << "error while getting the password to the DB: " << parseExcp.what() << "\n";
        logger << "\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);

    } catch (TRequestException& excp) {
        throw;

    } catch (std::exception& excp) {
        logger << TLevel::Fatal << "error while the setting the connection to the DB: " << excp.what();
        throw TRequestException(excp.what(), 401); 
    }
}


auto NRequest::TPostgresConnection::GetConnectionStatus() const noexcept {
    return PQstatus(Connection);
}


NRequest::TCacheManager::TCacheManager() {
    logger << TLevel::Info << "checking the existing of the table 'weather' begun "
        "(and creating it if not exists)\n";

    auto checkOrCreate = PQexec(Connection.GetConnection(), "CREATE TABLE IF NOT EXISTS weather ("
    "id SERIAL PRIMARY KEY, city VARCHAR(40), description VARCHAR(300), expires TIMESTAMP)");
    
    if (PQresultStatus(checkOrCreate) == PGRES_FATAL_ERROR ||
        PQresultStatus(checkOrCreate) == PGRES_NONFATAL_ERROR) 
    {
        PQclear(checkOrCreate);

        logger << TLevel::Fatal << "the query of creating/checking the relation ";
        logger << "wasn't processed correctly\n";
        logger << PQerrorMessage(Connection.GetConnection()) << "\n\n";

        throw TRequestException("the query of creating/checking the relation "
            "wasn't processed correctly", 504);
    }

    logger << TLevel::Info << "the relation was created/checked correctly\n\n";
    
    PQclear(checkOrCreate);
}


/// @return true if code is safety and false if symbols are suspicious
bool NRequest::TCacheManager::IsDataSafety(const std::string& city) const {
    logger << TLevel::Debug << "checking the availability of the SQL-injection in the city '";
    logger << city << "'begun\n";

    for (const auto& elem : city) {
        if (elem >= 65 && elem <= 90 || elem >= 97 && elem <= 122 || elem == '-') {
            continue;
        } else {
            logger << TLevel::Warning << "the data for the city '" << city << "' are suspicious\n\n";
            return false;
        }
    }

    logger << TLevel::Debug << "checking the SQL-injection in the city '" << city;
    logger << "'was finished correcly: the SQL-injections perhaps aren't in it\n\n";
    return true;
}


/// @return Return true if data aren't in the DB or it has the expired time status
/// if data hasn't expired time status function returns false 
/// PAY ATTENTION that the expires column is related to the time that defined as getting time + 15 mins;
bool NRequest::TCacheManager::IsDataExpired(const std::string& city) {
    logger << TLevel::Debug << "checking the data are in the DB and hasn't expired time status begun\n";

    if (!IsDataSafety(city)) {
        throw TRequestException("data are suspicious with SQL-injection", 405);
    }

    if (city.empty()) {
        logger << TLevel::Error << "~ IsDataExpired() warning: the city name isn't correct (empty)\n\n";
        throw TRequestException("the city name isn't correct (empty)", 401);
    }

    time_t sysCurrentTime = time(NULL);
    tm* currentUTCTime = gmtime(&sysCurrentTime);
    std::ostringstream timeUTCStream;

    timeUTCStream << (currentUTCTime->tm_year + 1900) << "-";
    timeUTCStream << (currentUTCTime->tm_mon + 1) << "-";
    timeUTCStream << currentUTCTime->tm_mday << " ";
    timeUTCStream << currentUTCTime->tm_hour << ":";
    timeUTCStream << currentUTCTime->tm_min << ":00";

    auto checkResult = PQexec(Connection.GetConnection(), 
        (std::string("SELECT * FROM weather\r\nWHERE city = '") + city +
         std::string("' AND expires >= '") + timeUTCStream.str() + "'\r\n").c_str());

    if (PQresultStatus(checkResult) == PGRES_FATAL_ERROR || 
        PQresultStatus(checkResult) == PGRES_NONFATAL_ERROR)
    {
        PQclear(checkResult);

        logger << TLevel::Error << "~ IsDataExpired() error: the query of the checking the city note ";
        logger << "wasn't processed correctly\n";
        logger << PQerrorMessage(Connection.GetConnection()) << "\n\n";

        throw TRequestException("the query of the checking the city note wasn't processed coorectly", 402);
    }
    
    logger << TLevel::Debug << "the query of the checking the city note finished correctly\n";
    if (!PQntuples(checkResult)) {
        PQclear(checkResult);
        logger << TLevel::Debug << "the note for the city '" << city << "' wasn't found\n\n";
        return true;
    }

    PQclear(checkResult);

    logger << TLevel::Debug << "the note for the city '" << city << "' was found correctly\n\n";
    return false;
}


std::string NRequest::TCacheManager::GetData(const std::string& city) {
    logger << TLevel::Debug << "getting the data from the DB has begun\n";

    if (!IsDataSafety(city)) {
        throw TRequestException("data are suspicious with SQL-injection", 405);
    }

    if (city.empty()) {
        logger << TLevel::Error << "~ IsDataExpired() warning: the city name isn't correct (empty)\n\n";
        throw TRequestException("the city name isn't correct (empty)", 401);
    }

    if (!IsDataExpired(city)) {
        auto checkGetResult = PQexec(Connection.GetConnection(), (std::string("SELECT * FROM weather")+
            "\r\nWHERE city='" + city + "'\r\n").c_str());

        if (PQresultStatus(checkGetResult) == PGRES_FATAL_ERROR || 
            PQresultStatus(checkGetResult) == PGRES_NONFATAL_ERROR)
        {
            PQclear(checkGetResult);

            logger << TLevel::Error << "~ GetData() error: the query of the getting the city note ";
            logger << "wasn't processed correctly\n";
            logger << PQerrorMessage(Connection.GetConnection()) << "\n\n";

            throw TRequestException("the query of the getting the city note wasn't processed coorectly", 402);
        }

        std::string cacheDesc = PQgetvalue(checkGetResult, 0, 2);

        logger << TLevel::Debug << "getting the data from the DB for the city '" << city;
        logger << "' finished correctly: the note about this city is existed and data has ";
        logger << "unexpired status\n\n";

        PQclear(checkGetResult);

        return cacheDesc;

    } else {
        logger << TLevel::Debug << "getting the data from the DB for the city '" << city;
        logger << "' finished correctly: the note about this city IS NOT existed or data has ";
        logger << "expired status\n\n";
        return "";
    }
}


bool NRequest::TCacheManager::CheckExistingData(const std::string& city) {
    logger << TLevel::Debug << "checking the existing of the city '" << city << "' begun\n";

    if (!IsDataSafety(city)) {
        throw TRequestException("data are suspicious with SQL-injection", 405);
    }

    if (city.empty()) {
        logger << TLevel::Error << "~ CheckExistingData() error: the city is empty\n\n";
        throw TRequestException("invalid data", 401);
    }

    auto checkResult = PQexec(Connection.GetConnection(), ("SELECT * FROM weather\r\n"
        "WHERE city='" + city + "'\r\n").c_str());
    
    if (PQresultStatus(checkResult) == PGRES_FATAL_ERROR || 
        PQresultStatus(checkResult) == PGRES_NONFATAL_ERROR)
    {
        PQclear(checkResult);

        logger << TLevel::Error << "~ CheckExistingData() error: the query of the checking the city's ";
        logger << "existing wasn't processed correctly\n";
        logger << PQerrorMessage(Connection.GetConnection()) << "\n\n";

        throw TRequestException("the query of the checking the city's existing wasn't processed coorectly", 402);
    }
    
    logger << TLevel::Debug << "the query of the checking the city's existing finished correctly\n";
    if (!PQntuples(checkResult)) {
        PQclear(checkResult);
        logger << TLevel::Debug << "the note for the city '" << city << "' wasn't found\n\n";
        return false;
    }

    PQclear(checkResult);

    logger << TLevel::Debug << "the note for the city '" << city << "' was found correctly\n\n";

    return true;
}


void NRequest::TCacheManager::InsertOrUpdateData(const std::string& city, const std::string& weathDesc) {
    logger << TLevel::Debug << "inserting/updating the data for the city '" << city << "' begun\n";

    if (!IsDataSafety(city)) {
        throw TRequestException("data are suspicious with SQL-injection", 405);
    }

    if (city.empty()) {
        logger << TLevel::Error << "~ IsDataExpired() warning: the city name isn't correct (empty)\n\n";
        throw TRequestException("the city name isn't correct (empty)", 401);
    }
    
    auto expiredTime = time(NULL) + 15 * 60;
    tm* expiredUTCTime = gmtime(&expiredTime);
    std::ostringstream requestExpiredTimeStream;

    requestExpiredTimeStream << (expiredUTCTime->tm_year + 1900) << "-";
    requestExpiredTimeStream << (expiredUTCTime->tm_mon + 1) << "-";
    requestExpiredTimeStream << expiredUTCTime->tm_mday << " ";
    requestExpiredTimeStream << expiredUTCTime->tm_hour << ":";
    requestExpiredTimeStream << expiredUTCTime->tm_min << ":00";

    if (CheckExistingData(city)) {
        auto updateResult = PQexec(Connection.GetConnection(), (std::string("UPDATE weather\r\n") +
            "SET description='" + weathDesc + "', expires='" + requestExpiredTimeStream.str() + "'\r\n" +
            "WHERE city='" + city + "'\r\n").c_str());
        
        if (PQresultStatus(updateResult) == PGRES_FATAL_ERROR || 
            PQresultStatus(updateResult) == PGRES_NONFATAL_ERROR)
        {
            PQclear(updateResult);

            logger << TLevel::Error << "~ InsertOrUpdate() error: the query of the updating the city's weather ";
            logger << "description wasn't processed correctly\n";
            logger << PQerrorMessage(Connection.GetConnection()) << "\n\n";

            throw TRequestException("the query of the updating the city's weather description wasn't processed coorectly", 402);
        }

        PQclear(updateResult);

        logger << TLevel::Debug << "the updating the weather description of the city '" << city << "' ";
        logger << "finished correctly\n\n";

    } else {
        auto insertResult = PQexec(Connection.GetConnection(), (std::string("INSERT INTO weather ") + 
            "(city, description, expires)\r\nVALUES ('" + city + "', '" + weathDesc + "', '" + 
            requestExpiredTimeStream.str() +"')\r\n").c_str());
        
        if (PQresultStatus(insertResult) == PGRES_FATAL_ERROR || 
            PQresultStatus(insertResult) == PGRES_NONFATAL_ERROR)
        {
            PQclear(insertResult);

            logger << TLevel::Error << "~ InsertOrUpdate() error: the query of the inserting the city's weather ";
            logger << "description wasn't processed correctly\n";
            logger << PQerrorMessage(Connection.GetConnection()) << "\n\n";

            throw TRequestException("the query of the inserting the city's weather description wasn't processed coorectly", 402);
        }

        PQclear(insertResult);

        logger << TLevel::Debug << "the inserting the weather description of the city '" << city << "' ";
        logger << "finished correctly\n\n";
    }
}