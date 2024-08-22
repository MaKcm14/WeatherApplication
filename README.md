# Weather_Application
Modern weather web-applications are complex and has too many functions that are not always need to get fast the current weather in the city or place you want to know.

So if you like simplicity and concept 'nothing extra' this application is for you!

This weather application lets to get the **current** weather for the cities in the Russian Federation.
<br>
<hr>
<hr>
<h2>Main Description</h2>
This application in its base configuration can get the current weather for cities in the Russian Federation.

<h3>Architecture of the application</h3>
It's a server-side web-application for getting the weather by request from a client using the Rest-concept (Rest service the second level of the maturity model).
The application was developed as expandable web-application with opportunity of adding other HTTP-methods and changing the strcuture of the front-end-part.

The application has *three main classes:*
- Weather_Service
- Cache_Manager
- Request_Manager

<h4>The Schema of application's architecture:</h4>

![](https://github.com/MaKcm14/Weather_Application/blob/master/weather_application_architecture_schema.jpg?raw=true "Architecture of the application")

<h5>Main steps of the interaction:</h5>
Client send the HTTP-message with GET-methd to get the main /find page and write the desired city in the input field after that the HTTP-message with POST-method.
This request processes by the weather_service which gets the weather description using the either cache_manager or request_manager module.

<h4>Weather_Service</h4>
Weather_Service is the main module that manages the request_manager module and cache_manager module.
It accepts the client's requests to connection and makes main interaction with the client.

<h4>Cache_Manager</h4>
Due to optimization there are the cache is used in the application. Cache_manager lets make handling the interaction with the cache
that can save info only on 15 mins after its getting.

<h4>Request_Manager</h4>
This module makes the interaction with the Meteo_Service (openweathermap.org) through the API (api.openweathermap.org).
It sends the HTTP-message with GET-method to get the weather in the city that was specified by the client.

<h4>P.S.</h4>
Every module of this application has own checklists that serve to prevent different unpredictable and innormal situations (as SQP-injection, for example).

<hr>
<h2>How to install and set the program?</h2>

Please ***read everything below here attentively*** because the service has complex installing and setting.

<h3>Installing.</h3>

1. For correct work you need to install the Boost, PostgreSQL and nlohmann-json libraries. You can install it the same way (Linux):

- `sudo apt install libboost-all-dev`
- https://www.postgresql.org/docs/current/tutorial-install.html
- https://github.com/nlohmann/json

2. After the installing the main components you need to build the project.

<h5>Use the next commands in the root directory of the repo to build and compile the project</h5>

- `mkdir build`
- `cd build`
- `cmake ..`
- If everything is OK: `cmake --build .`
- Else you need to fix the problems with CMake.

3. After the building and compiling the project you'll see the `main` target in the build directory.

After this steps the code will be ready to setting.
<br>
<h3>Setting.</h3>
<h4>Configuration.json</h4>

Weather_application stores the settings in the `configuration.json`.

It's really ***important file:*** without it you won't be able to use the application.

<h5>Format of the configuration.json:</h5>

```
{
  "API_KEY" : {
                "api.openweathermap.org" : "your_api_key_for_this_service"
              },
  "db_password" : "your_db_password"
}
```

- The value for key "api.openweathermap.org" is the API key that you need to get from the https://openweathermap.org/current
- The value for key "db_password" is your password for `postgres` role

<h5>Where the configuration.json must be?</h5>

You need to put this file **in the parent directory relatively the `build` directory.**

<h4>Tests</h4>
After completing the all steps from the title 'Configuration.json' you need to test all modules to make sure that they have the correct behaviour.

For using the tests **you need to put the 'configuration.json' in the directory `tests`**

For starting the tests you need to complete the next steps:
- go to the `build` directory
- `sudo service postgresql start`
- `ctest`

After starting the tests you can see the result of its completing.

**If all tests complete successfully the service was set correctly.**

<h4>Attention:</h4>
The Meteo-Service has really great load. Therefore the *request_manager_tests* can be failed. If it failed due to the Meteo-service problems you need to wait and try again later.

<br>
Log file is in the same directory as main.exe (main). It will be updated in every session you start.
<br>
<h2>How to use the application?</h2>
Here you can find the info about the administrating, using and starting the service.

