#include "include/cache_manager.h"

NRequest::TCacheManager::TCacheManager(std::unique_ptr<NDataBase::TDataBase> dataBase) 
    : DataBase(std::move(dataBase))
{
    logger << TLevel::Info << "connection to the DB was set correcly\n";
    logger << TLevel::Info << "checking the existing of the table 'weather' begun ";
    logger << "(and creating it if not exists)\n";

    try {
        DataBase->ICreateTable("CREATE TABLE IF NOT EXISTS weather ("
            "id SERIAL PRIMARY KEY, city VARCHAR(40), description VARCHAR(300), expires TIMESTAMP)");

    } catch (NDataBase::TDataBaseException& excp) {
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


/// @param shift in the mins 
std::string NRequest::TCacheManager::GetCurTimeWithShift(int32_t shift) const {
    auto newTime = time(NULL) + shift * 60;
    tm* newUTCTime = gmtime(&newTime);
    std::ostringstream timeStream;

    timeStream << (newUTCTime->tm_year + 1900) << "-";
    timeStream << (newUTCTime->tm_mon + 1) << "-";
    timeStream << newUTCTime->tm_mday << " ";
    timeStream << newUTCTime->tm_hour << ":";
    timeStream << newUTCTime->tm_min << ":00";

    return timeStream.str();
}


/// @brief can be used ONLY AFTER call the IsDataExpired() method: 
std::string NRequest::TCacheManager::GetData(const std::string& city) {
    logger << TLevel::Debug << "getting the data from the DB has begun\n";

    if (!Checker.IsDataSafety(city)) {
        CacheNote.Clear();
        throw TRequestException("data are suspicious with SQL-injection", 405);
    }

    if (city.empty()) {
        CacheNote.Clear();
        logger << TLevel::Error << "~ IsDataExpired() warning: the city name isn't correct (empty)\n\n";
        throw TRequestException("the city name isn't correct (empty)", 401);
    }

    logger << TLevel::Debug << "getting the data from the DB for the city '" << city;
    logger << "' finished correctly\n\n";
    
    return CacheNote.CacheWeatherDesc;
}


/// @return Return true if data aren't in the DB or it has the expired time status
/// if data hasn't expired time status function returns false 
/// PAY ATTENTION that the expires column is related to the time that defined as getting time + 15 mins;
bool NRequest::TCacheManager::IsDataExpired(const std::string& city) {
    logger << TLevel::Debug << "checking the status of the descrpition ";
    logger << "for the city '" << city << "' begun\n";

    if (!Checker.IsDataSafety(city)) {
        CacheNote.Clear();
        throw TRequestException("data are suspicious with SQL-injection", 405);
    }

    if (city.empty()) {
        CacheNote.Clear();
        logger << TLevel::Error << "~ IsDataExpired() error: the city is empty\n\n";
        
        throw TRequestException("invalid data", 401);
    }

    if (CacheNote.City == city && CacheNote.CheckExistingFlag 
        && CacheNote.ExpiredTime >= GetCurTimeWithShift(0))
    {
        logger << TLevel::Debug << "the note for the city '" << city << "' has unexpired status\n\n";
        return !CacheNote.CheckExistingFlag;

    } else if (CacheNote.City == city && CacheNote.CheckExistingFlag &&
               CacheNote.ExpiredTime < GetCurTimeWithShift(0)) 
    {
        logger << TLevel::Debug << "the note for the city '" << city << "' ";
        logger << "the description has expired time status\n\n";
        return CacheNote.CheckExistingFlag;
    }

    CacheNote.Clear();
    CacheNote.City = city;

    std::vector<std::vector<std::string>> existingData;

    try {
        existingData = DataBase->ISelect("SELECT * FROM weather\r\n"
            "WHERE city='" + city + "'\r\n");
    
    } catch (NDataBase::TDataBaseException& excp) {
        CacheNote.Clear();
        logger << TLevel::Error << "~ IsDataExpired() error: the query of the checking the city's ";
        logger << "description status wasn't processed correctly: " << excp.what() << "\n\n";
        
        throw TRequestException("the query of the checking the city's description status "
            "wasn't processed coorectly", 402);
    }
    
    logger << TLevel::Debug << "the query of the checking the city's description status ";
    logger << "finished correctly\n";
    
    if (existingData.empty() || existingData[0][3] < GetCurTimeWithShift(0)) {
        
        ///DEBUG:
        std::cout << existingData[0][3] << " " << GetCurTimeWithShift(0) << std::endl;
        ///

        logger << TLevel::Debug << "the note for the city '" << city << "' wasn't found/";
        logger << "the description has expired time status\n\n";
        return true;
    } else {
        CacheNote.CacheWeatherDesc = std::move(existingData[0][2]);
        CacheNote.CheckExistingFlag = true;
        CacheNote.ExpiredTime = std::move(existingData[0][3]);
    }

    logger << TLevel::Debug << "the note for the city was found and '" << city << "' ";
    logger << "has unexpired status\n\n";

    return false;
}


void NRequest::TCacheManager::UpdateCache(const std::string& city, const std::string& weathDesc) {
    try {
        std::string expiredTime = GetCurTimeWithShift(15);

        DataBase->IUpdate(std::string("UPDATE weather\r\n") +
            "SET description='" + weathDesc + "', expires='" + expiredTime + "'\r\n" +
            "WHERE city='" + city + "'\r\n");

    } catch (NDataBase::TDataBaseException& excp) {
        CacheNote.Clear();

        logger << TLevel::Error << "~ UpdateCache() error: the query of the updating";
        logger << " the city's weather description wasn't processed correctly: ";
        logger << excp.what() << "\n\n";
            
        throw TRequestException("the query of the updating the city's weather description "
            "wasn't processed coorectly", 402);
    }
}


void NRequest::TCacheManager::InsertCache(const std::string& city, const std::string& weathDesc) {
    try {
        std::string expiredTime = GetCurTimeWithShift(15);
            
        DataBase->IInsert(std::string("INSERT INTO weather ") + 
            "(city, description, expires)\r\nVALUES ('" + city + "', '" + weathDesc + "', '" + 
            expiredTime +"')\r\n");
        
    } catch (NDataBase::TDataBaseException& excp) {
        CacheNote.Clear();

        logger << TLevel::Error << "~ InsertCache() error: the query of the inserting ";
        logger << "the city's weather description wasn't processed correctly: ";
        logger << excp.what() << "\n\n";
            
        throw TRequestException("the query of the inserting the city's weather description "
            "wasn't processed coorectly", 402);
    }
}


/// @brief can be used ONLY AFTER call the IsDataExpired() method: 
void NRequest::TCacheManager::InsertOrUpdateData(const std::string& city, const std::string& weathDesc) {
    logger << TLevel::Debug << "inserting/updating the data for the city '" << city << "' begun\n";

    if (!Checker.IsDataSafety(city)) {
        CacheNote.Clear();
        throw TRequestException("data are suspicious with SQL-injection", 405);
    }

    if (city.empty() || weathDesc.empty()) {
        CacheNote.Clear();
        logger << TLevel::Error << "~ IsDataExpired() warning: the city name/weather description "
            "isn't correct (empty)\n\n";
        
        throw TRequestException("the city name/weather description isn't correct (empty)", 401);
    }

    if (CacheNote.City == city && CacheNote.CheckExistingFlag) {
        UpdateCache(city, weathDesc);
        
        logger << TLevel::Debug << "the updating the weather description of the city '" << city << "' ";
        logger << "finished correctly\n\n";

    } else if (CacheNote.City == city) {
        InsertCache(city, weathDesc);
        
        logger << TLevel::Debug << "the inserting the weather description of the city '" << city << "' ";
        logger << "finished correctly\n\n";

    } else {
        logger << TLevel::Error << "~ InsertOrUpdateData(): the wrong name of the city was got after ";
        logger << "the call IsDataExpired()\n\n";
        throw TRequestException("the wrong city was got", 401);
    }

    CacheNote.City = city;
    CacheNote.CacheWeatherDesc = weathDesc;
    CacheNote.CheckExistingFlag = true;
}
