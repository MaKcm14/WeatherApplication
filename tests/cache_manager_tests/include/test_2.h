#include "cache_manager.h"
#include "gtest/gtest.h"
#include "test_postgres_orm.h"

TEST(Cache_Manager_Tests, Check_Getting_Empty_Data) {
    NRequest::TCacheManager testCache(
        std::make_unique<NDataBase::TPostgreSql>("127.0.0.1", "5432", 
        NRequest::configJson.at("db_password").dump())
    );

    try {
        testCache.GetData("");

    } catch (NRequest::TRequestException& excp) {
        ASSERT_EQ(excp.GetErrorId(), 401) << "the wrong errorID for the exception\n\n";

    } catch (...) {
        FAIL() << "the unpredictable exception was generated\n\n";
    }
}


TEST(Cache_Manager_Tests, Check_Expiring_The_Empty_Data) {
    NRequest::TCacheManager testCache(std::make_unique<NDataBase::TPostgreSql>("127.0.0.1", "5432", 
        NRequest::configJson.at("db_password").dump())
    );

    try {
        testCache.IsDataExpired("");

    } catch (NRequest::TRequestException& excp) {
        ASSERT_EQ(excp.GetErrorId(), 401) << "the wrong errorID for the exception\n\n";

    } catch (...) {
        FAIL() << "the unpredictable exception was generated\n\n";
    }
}


TEST(Cache_Manager_Tests, Check_Inserting_And_Updating_Empty_Data) {
    NRequest::TCacheManager testCache(std::make_unique<NDataBase::TPostgreSql>("127.0.0.1", "5432", 
        NRequest::configJson.at("db_password").dump())
    );

    try {
        testCache.InsertOrUpdateData("", "");

    } catch (NRequest::TRequestException& excp) {
        ASSERT_EQ(excp.GetErrorId(), 401) << "the wrong errorID for the exception\n\n";

    } catch (...) {
        FAIL() << "the unpredictable exception was generated\n\n";
    }
}

/// this test can drop your table:
TEST(Cache_Manager_Tests, Check_Getting_The_Suspicious_Data) {
    try {
        TTestPostgres connect;
        NRequest::TCacheManager testCache(std::make_unique<NDataBase::TPostgreSql>("127.0.0.1", "5432", 
            NRequest::configJson.at("db_password").dump())
        );
        
        connect.Insert("INSERT INTO weather (city, description, expires)\r\n"
                       "VALUES ('Chekalin', 'Beautiful', '" + connect.GetCurTimeWithShift(5) + "')\r\n");


        ASSERT_THROW(testCache.IsDataExpired("' or 1=1--"), NRequest::TRequestException) << "the wrong "
            "exception for SQL-injection query\n\n";

        ASSERT_THROW(testCache.IsDataExpired("' DROP TABLE weather--"); , NRequest::TRequestException) << "the wrong "
            "exception for SQL-injection query\n\n";

        ASSERT_THROW(testCache.InsertOrUpdateData("' DROP TABLE weather--", "weather");, NRequest::TRequestException) << "the wrong "
            "exception for SQL-injection query\n\n";


        auto result = connect.Select("SELECT * FROM weather\r\nWHERE city='Chekalin'\r\n");

        connect.Delete("DELETE FROM weather\r\nWHERE city='Chekalin'\r\n");

        ASSERT_EQ(result[1], "Chekalin") << "the SQL-injection was processed CORRECTLY\n\n";

    } catch (...) {
        FAIL() << "the unpredictable error was generated\n\n";
    }
}

