#include "include/test_postgres_orm.h"

namespace NRequest {
    extern nlohmann::json configJson;
}

TPostgresConnection::TPostgresConnection() {
     try {
        std::string password = NRequest::configJson.at("db_password").dump();

        Connection = PQsetdbLogin("127.0.0.1", "5432", nullptr, nullptr, "postgres", "postgres", 
            password.substr(1, password.size() - 2).c_str());

        if (PQstatus(Connection) == CONNECTION_BAD) {
            throw NDataBase::TDataBaseException("connection to the DB wasn't set correctly");
        } 

    } catch (nlohmann::json::type_error& typeExcp) {
        throw NRequest::TRequestException(typeExcp.what(), 502);

    } catch (nlohmann::json::parse_error& parseExcp) {
        throw NRequest::TRequestException(parseExcp.what(), 502);

    } catch (std::exception& excp) {
        throw NDataBase::TDataBaseException(excp.what()); 
    }
}


/// @param shift in the mins 
std::string TTestPostgres::GetCurTimeWithShift(int32_t shift) {
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


std::vector<std::string> TTestPostgres::Select(const std::string& query) {
    std::vector<std::string> data;
    auto result = PQexec(Connection, query.c_str());

    if (PQresultStatus(result) == PGRES_FATAL_ERROR || 
        PQresultStatus(result) == PGRES_NONFATAL_ERROR)
    {
        PQclear(result);
        throw NRequest::TRequestException("error while the processing the 'SELECT' query", 404);
    }

    if (PQntuples(result)) {
        for (size_t i = 0; i != 4; ++i) {
            data.push_back(PQgetvalue(result, 0, i));
        }

    } else {
        data.resize(4);
    }

    PQclear(result);

    return data;
}


void TTestPostgres::Insert(const std::string& query) {
    auto result = PQexec(Connection, query.c_str());

    if (PQresultStatus(result) == PGRES_FATAL_ERROR || 
        PQresultStatus(result) == PGRES_NONFATAL_ERROR)
    {
        PQclear(result);
        throw NRequest::TRequestException("error while the processing the 'INSERT' query", 404);
    }

    PQclear(result);
}


void TTestPostgres::Update(const std::string& query) {
    auto result = PQexec(Connection, query.c_str());

    if (PQresultStatus(result) == PGRES_FATAL_ERROR || 
        PQresultStatus(result) == PGRES_NONFATAL_ERROR)
    {
        PQclear(result);
        throw NRequest::TRequestException("error while the processing the 'UPDATE' query", 404);
    }

    PQclear(result);
}


void TTestPostgres::Delete(const std::string& query) {
    auto result = PQexec(Connection, query.c_str());

    if (PQresultStatus(result) == PGRES_FATAL_ERROR || 
        PQresultStatus(result) == PGRES_NONFATAL_ERROR)
    {
        PQclear(result);
        throw NRequest::TRequestException("error while the processing the 'DELETE' query", 404);
    }

    PQclear(result);
}