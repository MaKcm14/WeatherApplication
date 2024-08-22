# Weather_Application
Weather application that lets to get weather for the cities in the Russian Federation.
<br>
<hr>
<hr>
<h2>Main Description</h2>
This application in its base configuration can get weather for cities in the Russian Federation.

<h3>Architecture of the application</h3>
It's a server-side web-application for getting the weather by request from a client using the Rest-concept (Rest service the second level of the maturity model).
The application was developed as expandable web-application with opportunity of adding other HTTP-methods and changing the strcuture of the front-end-part.

The application has *three main classes:*
- Weather_Service
- Cache_Manager
- Request_Manager

<h4>The Schema of application's architecture:</h4>

![](https://github.com/MaKcm14/Weather_Application/blob/master/weather_application_architecture_schema.jpg?raw=true "Architecture of the application")

<hr>
<hr>
<h2>How to install and set the program?</h2>
<h3>Installing.</h3>
For correct work you need to install the Boost and PostgreSQL libraries. You can install it the same way (Linux):

- sudo apt install libboost-all-dev
- https://www.postgresql.org/docs/current/tutorial-install.html
<br>
<h3>Setting.</h3>
For correct work you need to create a 'configuration.json' in the parent directory using the next format:
<br>
<br>{
<br>    "API_KEY" : {
<br>        "api.openweathermap.org" : "your_api_key_for_this_service"
<br>    },
<br>    "db_password" : "your_db_password"
<br>}
<br>
<br>
For using the tests you need to put the copy of the 'configuration.json' in the parent directory
of the tests (directory 'test' in 'build').
<br>
Log file is in the same directory as main.exe (main). It will be updated in every session you start.
<br>
<h2>How to use the application?</h2>
Before start you need to start the postgresql server.
For using the weather_application you need to execute the main.exe (main) file that you'll get after compiling the project.
