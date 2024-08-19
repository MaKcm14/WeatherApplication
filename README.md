Weather_Application

This project can get weather for cities in the Russian Federation.
It's a server-side web-application for getting the weather by request from a client using the Rest-concept (Rest service the second level of the maturity model).

P.S.
~ For correct work you need to create a 'configuration.json' in the parent directory using the next format:
{
    "API_KEY" : {
        "api.openweathermap.org" : "your_api_key_for_this_service"
    },
    "db_password" : "your_db_password"
}

~ Also you need to install PostgreSql and Boost library correctly.

~ For using the tests you need to put the copy of the 'configuration.json' in the parent directory
of the tests (directory 'test' in 'build').

~ Log file is in the same directory as main.exe (main). It will be updated in every session you start.
