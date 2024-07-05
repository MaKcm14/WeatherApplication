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
        logger << TLevel::Fatal << "error while setting the password to the DB: " << typeExcp.what() << "\n";
        logger << TLevel::Fatal << "\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Fatal << "error while setting the password to the DB: " << parseExcp.what() << "\n";
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
        logger << TLevel::Fatal << "connection to the DB wasn't set\n\n";
        throw TRequestException("connection to the DB wasn't set correctly", 403);
    }

    logger << TLevel::Info << "connection to the DB was set\n\n";
    logger << TLevel::Info << "checking the existing of the table weather (and create it if not exists)\n";

    auto checkOrCreate = PQexec(Connection.GetConnection(), "CREATE TABLE IF NOT EXISTS weather ("
    "id SERIAL PRIMARY KEY, city VARCHAR(40), description VARCHAR(300), last_modified DATE)");

}

/// TODO: add functions for getting (+ checking) and writing the needed data 