#include "include/cache_manager.h"

NRequest::TCacheManager::TCacheManager(std::unique_ptr<NDataBase::TDataBase> dataBase) 
    : DataBase(std::move(dataBase))
{
    logger << TLevel::Info << "connection to the DB was set correcly\n";
    
    logger << TLevel::Info << "checking the existing of the table 'weather' begun "
        "(and creating it if not exists)\n";

    try {
        DataBase->CreateTable("CREATE TABLE IF NOT EXISTS weather ("
            "id SERIAL PRIMARY KEY, city VARCHAR(40), description VARCHAR(300), expires TIMESTAMP)");

    } catch (NRequest::TRequestException& excp) {
        logger << TLevel::Fatal << "the query of creating/checking the relation ";
        logger << "wasn't processed correctly: " << excp.what() << "\n\n";
        throw TRequestException("the query of creating/checking the relation "
            "wasn't processed correctly", 504);
    }

    logger << TLevel::Info << "the relation was created/checked correctly\n\n";
}


/// @return true if code is safety and false if symbols are suspicious
bool NRequest::TDataChecker::IsDataSafety(const std::string& city) const {
    logger << TLevel::Debug << "checking the availability of the SQL-injection in the city '";
    logger << city << "' begun\n";

    for (const auto& elem : city) {
        if (elem >= 65 && elem <= 90 || elem >= 97 && elem <= 122 || elem == '-' || elem == ' ') {
            continue;
        } else {
            logger << TLevel::Warning << "the data for the city '" << city << "' are suspicious\n\n";
            return false;
        }
    }

    logger << TLevel::Debug << "checking the SQL-injection in the city '" << city;
    logger << "' was finished correcly: the SQL-injections perhaps aren't in it\n\n";
    return true;
}


/// @return Return true if data aren't in the DB or it has the expired time status
/// if data hasn't expired time status function returns false 
/// PAY ATTENTION that the expires column is related to the time that defined as getting time + 15 mins;
bool NRequest::TCacheManager::IsDataExpired(const std::string& city) {
    logger << TLevel::Debug << "checking the data are in the DB and hasn't expired time status begun\n";

    if (!Checker.IsDataSafety(city)) {
        throw TRequestException("data are suspicious with SQL-injection", 405);
    }

    if (city.empty()) {
        logger << TLevel::Error << "~ IsDataExpired() warning: the city name isn't correct (empty)\n\n";
        throw TRequestException("the city name isn't correct (empty)", 401);
    }

    time_t sysCurrentTime = time(NULL);
    tm* currentUTCTime = gmtime(&sysCurrentTime);
    std::ostringstream timeUTCStream;
    std::vector<std::vector<std::string>> selectResult;

    timeUTCStream << (currentUTCTime->tm_year + 1900) << "-";
    timeUTCStream << (currentUTCTime->tm_mon + 1) << "-";
    timeUTCStream << currentUTCTime->tm_mday << " ";
    timeUTCStream << currentUTCTime->tm_hour << ":";
    timeUTCStream << currentUTCTime->tm_min << ":00";

    try {
        selectResult = DataBase->Select(std::string("SELECT * FROM weather\r\nWHERE city = '") + city +
            std::string("' AND expires >= '") + timeUTCStream.str() + "'\r\n");
        
    } catch (NDataBase::TDataBaseException& excp) {
        logger << TLevel::Error << "~ IsDataExpired() error: the query of the checking the city note ";
        logger << "wasn't processed correctly: " << excp.what() << "\n\n";
        throw TRequestException("the query of the checking the city note wasn't processed coorectly", 402);
    }
    
    logger << TLevel::Debug << "the query of the checking the city note finished correctly\n";

    if (selectResult.empty()) {
        logger << TLevel::Debug << "the note for the city '" << city << "' wasn't found\n\n";
        return true;
    }

    logger << TLevel::Debug << "the note for the city '" << city << "' was found correctly\n\n";

    return false;
}


std::string NRequest::TCacheManager::GetData(const std::string& city) {
    logger << TLevel::Debug << "getting the data from the DB has begun\n";

    if (!Checker.IsDataSafety(city)) {
        throw TRequestException("data are suspicious with SQL-injection", 405);
    }

    if (city.empty()) {
        logger << TLevel::Error << "~ IsDataExpired() warning: the city name isn't correct (empty)\n\n";
        throw TRequestException("the city name isn't correct (empty)", 401);
    }

    if (!IsDataExpired(city)) {
        std::vector<std::vector<std::string>> selectResult; 
        
        try {
            selectResult = DataBase->Select(std::string("SELECT * FROM weather")+
                "\r\nWHERE city='" + city + "'\r\n");

        } catch (NDataBase::TDataBaseException& excp) {
            logger << TLevel::Error << "~ GetData() error: the query of the getting the city note ";
            logger << "wasn't processed correctly: " << excp.what() << "\n\n";
            throw TRequestException("the query of the getting the city note wasn't processed coorectly", 402);
        }

        std::string cacheDesc = selectResult[0][2];

        logger << TLevel::Debug << "getting the data from the DB for the city '" << city;
        logger << "' finished correctly: the note about this city is existed and data has ";
        logger << "unexpired status\n\n";

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

    if (!Checker.IsDataSafety(city)) {
        throw TRequestException("data are suspicious with SQL-injection", 405);
    }

    if (city.empty()) {
        logger << TLevel::Error << "~ CheckExistingData() error: the city is empty\n\n";
        throw TRequestException("invalid data", 401);
    }

    std::vector<std::vector<std::string>> selectResult;

    try {
        selectResult = DataBase->Select("SELECT * FROM weather\r\n"
            "WHERE city='" + city + "'\r\n");
    
    } catch (NDataBase::TDataBaseException& excp) {
        logger << TLevel::Error << "~ CheckExistingData() error: the query of the checking the city's ";
        logger << "existing wasn't processed correctly: " << excp.what() << "\n\n";
        throw TRequestException("the query of the checking the city's existing wasn't processed coorectly", 402);
    }
    
    logger << TLevel::Debug << "the query of the checking the city's existing finished correctly\n";
    
    if (selectResult.empty()) {
        logger << TLevel::Debug << "the note for the city '" << city << "' wasn't found\n\n";
        return false;
    }

    logger << TLevel::Debug << "the note for the city '" << city << "' was found correctly\n\n";

    return true;
}


void NRequest::TCacheManager::InsertOrUpdateData(const std::string& city, const std::string& weathDesc) {
    logger << TLevel::Debug << "inserting/updating the data for the city '" << city << "' begun\n";

    if (!Checker.IsDataSafety(city)) {
        throw TRequestException("data are suspicious with SQL-injection", 405);
    }

    if (city.empty() || weathDesc.empty()) {
        logger << TLevel::Error << "~ IsDataExpired() warning: the city name/weather description "
            "isn't correct (empty)\n\n";
        throw TRequestException("the city name/weather description isn't correct (empty)", 401);
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
        try {
            DataBase->Update(std::string("UPDATE weather\r\n") +
                "SET description='" + weathDesc + "', expires='" + requestExpiredTimeStream.str() + "'\r\n" +
                "WHERE city='" + city + "'\r\n");

        } catch (NDataBase::TDataBaseException& excp) {
            logger << TLevel::Error << "~ InsertOrUpdate() error: the query of the updating the city's weather ";
            logger << "description wasn't processed correctly: " << excp.what() << "\n\n";
            throw TRequestException("the query of the updating the city's weather description wasn't processed coorectly", 402);
        }

        logger << TLevel::Debug << "the updating the weather description of the city '" << city << "' ";
        logger << "finished correctly\n\n";

    } else {
        try {
            DataBase->Insert(std::string("INSERT INTO weather ") + 
                "(city, description, expires)\r\nVALUES ('" + city + "', '" + weathDesc + "', '" + 
                requestExpiredTimeStream.str() +"')\r\n");
        
        } catch (NDataBase::TDataBaseException& excp) {
            logger << TLevel::Error << "~ InsertOrUpdate() error: the query of the inserting the city's weather ";
            logger << "description wasn't processed correctly: "<< excp.what() << "\n\n";
            throw TRequestException("the query of the inserting the city's weather description wasn't processed coorectly", 402);
        }

        logger << TLevel::Debug << "the inserting the weather description of the city '" << city << "' ";
        logger << "finished correctly\n\n";
    }
}
