#include "cache_manager.h"
#include "gtest/gtest.h"
#include "test_postgres_orm.h"

TEST(Cache_Manager_Tests, Check_Expiring_The_Expired_Data) {
    try {
        TTestPostgres connect;
        NRequest::TCacheManager testCache(
            std::make_unique<NDataBase::TPostgreSql>("127.0.0.1", "5432", 
            NRequest::configJson.at("db_password").dump())
        );

        connect.Insert("INSERT INTO weather (city, description, expires)\r\n"
                       "VALUES ('Chekalin', 'Beautiful', '" + connect.GetCurTimeWithShift(-5) + "')\r\n");
        
        bool result = testCache.IsDataExpired("Chekalin");

        connect.Delete("DELETE FROM weather\r\nWHERE city='Chekalin'\r\n");

        ASSERT_TRUE(result) << "got the result: "
            "data is unexpired when the description already has expired\n";

    } catch (NRequest::TRequestException& excp) {
        FAIL() << "the exception was generated while normal conditions: " << excp.what() << "\n\n";

    } catch (...) {
        FAIL() << "the unpredictable error was generated\n\n";
    }
}


TEST(Cache_Manager_Tests, Check_Expiring_The_Unexpired_Data) {
    try {
        TTestPostgres connect;
        NRequest::TCacheManager testCache(
            std::make_unique<NDataBase::TPostgreSql>("127.0.0.1", "5432", 
            NRequest::configJson.at("db_password").dump())
        );

        connect.Insert("INSERT INTO weather (city, description, expires)\r\n"
                       "VALUES ('Chekalin', 'Beautiful', '" + connect.GetCurTimeWithShift(5) + "')\r\n");
        
        bool result = testCache.IsDataExpired("Chekalin");

        connect.Delete("DELETE FROM weather\r\nWHERE city='Chekalin'\r\n");

        ASSERT_FALSE(result) << "got the result: "
            "data is expired when the description is not expired yet\n";

    } catch (NRequest::TRequestException& excp) {
        FAIL() << "the exception was generated while normal conditions: " << excp.what() << "\n\n";

    } catch (...) {
        FAIL() << "the unpredictable error was generated\n\n";
    }
}


TEST(Cache_Manager_Tests, Check_Expiring_The_Unexisting_Data) {
    try {
        NRequest::TCacheManager testCache(
            std::make_unique<NDataBase::TPostgreSql>("127.0.0.1", "5432", 
            NRequest::configJson.at("db_password").dump())
        );

        bool result = testCache.IsDataExpired("Chekalin");

        ASSERT_TRUE(result) << "got the result: "
            "data is unexpired when the description is not existed\n";

    } catch (NRequest::TRequestException& excp) {
        FAIL() << "the exception was generated while normal conditions: " << excp.what() << "\n\n";

    } catch (...) {
        FAIL() << "the unpredictable error was generated\n\n";
    }
}


TEST(Cache_Manager_Tests, Check_Inserting_The_Data) {
     try {
        TTestPostgres connect;
        NRequest::TCacheManager testCache(
            std::make_unique<NDataBase::TPostgreSql>("127.0.0.1", "5432", 
            NRequest::configJson.at("db_password").dump())
        );

        testCache.IsDataExpired("Chekalin");
        testCache.InsertOrUpdateData("Chekalin", "Beautiful");

        auto result = connect.Select("SELECT * FROM weather\r\nWHERE city='Chekalin'\r\n");

        ASSERT_FALSE(result[1].empty()) << "the data are empty after inserting";
        ASSERT_FALSE(result[2].empty()) << "the data are empty after inserting";
        ASSERT_EQ(result[1], std::string("Chekalin")) << "the data are incorrect after inserting";
        ASSERT_EQ(result[2], std::string("Beautiful")) << "the data are incorrect after inserting";

        connect.Delete("DELETE FROM weather\r\nWHERE city='Chekalin'\r\n");

    } catch (NRequest::TRequestException& excp) {
        FAIL() << "the exception was generated while normal conditions: " << excp.what() << "\n\n";

    } catch (...) {
        FAIL() << "the unpredictable error was generated\n\n";
    }
}


TEST(Cache_Manager_Tests, Check_Updating_The_Data) {
   try {
        TTestPostgres connect;
        NRequest::TCacheManager testCache(
            std::make_unique<NDataBase::TPostgreSql>("127.0.0.1", "5432", 
            NRequest::configJson.at("db_password").dump())
        );

        connect.Insert("INSERT INTO weather (city, description, expires)\r\n"
                       "VALUES ('Chekalin', 'Beautiful', '" + connect.GetCurTimeWithShift(5) + "')\r\n");

        testCache.IsDataExpired("Chekalin");
        testCache.InsertOrUpdateData("Chekalin", "Nice");

        auto result = connect.Select("SELECT * FROM weather\r\nWHERE city='Chekalin'\r\n");

        ASSERT_FALSE(result[1].empty()) << "the data are empty after inserting";
        ASSERT_FALSE(result[2].empty()) << "the data are empty after inserting";
        ASSERT_EQ(result[1], std::string("Chekalin")) << "the data are incorrect after inserting";
        ASSERT_EQ(result[2], std::string("Nice")) << "the data are incorrect after inserting";

        connect.Delete("DELETE FROM weather\r\nWHERE city='Chekalin'\r\n");

    } catch (NRequest::TRequestException& excp) {
        FAIL() << "the exception was generated while normal conditions: " << excp.what() << "\n\n";

    } catch (...) {
        FAIL() << "the unpredictable error was generated\n\n";
    }
}


TEST(Cache_Manager_Tests, Check_Getting_Existing_Data) {
    try {
        TTestPostgres connect;
        NRequest::TCacheManager testCache(
            std::make_unique<NDataBase::TPostgreSql>("127.0.0.1", "5432", 
            NRequest::configJson.at("db_password").dump())
        );

        connect.Insert("INSERT INTO weather (city, description, expires)\r\n"
                       "VALUES ('Chekalin', 'Beautiful', '" + connect.GetCurTimeWithShift(5) + "')\r\n");

        testCache.IsDataExpired("Chekalin");
        std::string weatherRes = testCache.GetData("Chekalin");

        ASSERT_EQ(weatherRes, std::string("Beautiful")) << "the data are incorrect using 'GetData()'";

        connect.Delete("DELETE FROM weather\r\nWHERE city='Chekalin'\r\n");

    } catch (NRequest::TRequestException& excp) {
        FAIL() << "the exception was generated while normal conditions: " << excp.what() << "\n\n";

    } catch (...) {
        FAIL() << "the unpredictable error was generated\n\n";
    }
}


TEST(Cache_Manager_Tests, Check_Getting_Unexisting_Data) {
    try {
        NRequest::TCacheManager testCache(
            std::make_unique<NDataBase::TPostgreSql>("127.0.0.1", "5432", 
            NRequest::configJson.at("db_password").dump())
        );
        std::string weatherRes = testCache.GetData("Chekalin");

        ASSERT_EQ(weatherRes, std::string("")) << "the behaviour of the function" 
            "is unpredictable";

    } catch (NRequest::TRequestException& excp) {
        FAIL() << "the exception was generated while normal conditions: " << excp.what() << "\n\n";

    } catch (...) {
        FAIL() << "the unpredictable error was generated\n\n";
    }
}

