#include "./include/weather_service.h"

TLogger logger;

namespace NWeather {
    bool criticalError = false;
}


/// @brief converts the weather description in the html format 
std::string NWeather::TWeatherService::ConvertWeatherToHtml(const std::string& weather) const {
    std::ifstream htmlTemplateStream("../source/weather_service/resource/weather_template.txt");
    std::string weatherHtmlPage;

    if (!htmlTemplateStream.is_open()) {
        logger << TLevel::Fatal << "can't open the template weather file\n\n";
        throw NRequest::TRequestException("can't open the template weather file", 402);
    }

    for (char symb; htmlTemplateStream.get(symb); ) {
        weatherHtmlPage.push_back(symb);
    }

    weatherHtmlPage += weather;
    weatherHtmlPage += std::string("</font></center></body></html>");

    return weatherHtmlPage;
}


/// @brief parses the query from the client to realize what do the server need to do 
NWeather::TQueryParams NWeather::TWeatherService::ParseQuery(const std::string& query) const {
    logger << TLevel::Debug << "parsing the query from the client begun\n";

    TQueryParams queryParam;

    queryParam.Method = query.find("GET") < query.size() ? "GET" : "POST";

    queryParam.Resource = query.substr(query.find(" ") + 1, query.find(" ", query.find(" ") + 1) - 
        query.find(" ") - 1);

    std::string correctData;
    size_t nullIndex = query.size() + 1;

    for (size_t i = 0; i != query.size(); ++i) {
        if (!static_cast<int32_t>(query[i])) {
            nullIndex = i;
            break;
        }
    }

    queryParam.Data = query.substr(query.find("\r\n\r\n") + 4, nullIndex - query.find("\r\n\r\n") - 4);

    for (const auto& elem : queryParam.Data) {
        if (elem == '+') {
            correctData.push_back(' ');
        } else {
            correctData.push_back(elem);
        }
    }

    queryParam.Data = std::move(correctData);

    logger << TLevel::Debug << "parsing the query from the client was finished correctly\n\n";

    return queryParam;
}


/// @brief handles the GET query and returns the html resource  
std::string NWeather::TWeatherService::GetQueryHandler(const std::string& resource) const {
    logger << TLevel::Debug << "handling the 'GET' query for the resource '" << resource << "' begun\n";

    std::string htmlPage;
    std::ostringstream queryGetResponse;
    std::ifstream resourceStream;

    try {
        resourceStream.open(std::string("../source/weather_service/resource") + ResourcesToFiles.at(resource));
    
    } catch (std::exception& excp) {
        resourceStream.close();
    }

    if (!resourceStream.is_open()) {
        logger << TLevel::Warning << "the file '" << (resource.substr(1) + ".txt") << "' wasn't opened (or it doesn't exist)\n\n";
        
        queryGetResponse << ErrorCodesAndResponses.at("404");
        queryGetResponse << "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1>" \
                    "<p>The requested URL was not found on this server.</p></body></html>\r\n";
        
        return queryGetResponse.str();
    }

    for (char symb; resourceStream.get(symb); ) {
        htmlPage.push_back(symb);
    }

    queryGetResponse << "HTTP/1.1 200 OK\r\n";
    queryGetResponse << "Cache-Control: no-store\r\n";
    queryGetResponse << "Server: 127.0.0.1:8080\r\n";
    queryGetResponse << "Content-Length: " << htmlPage.size() << "\r\n";
    queryGetResponse << "Connection: Closed\r\n";
    queryGetResponse << "Content-Type: text/html; charset=ascii\r\n\r\n";
    queryGetResponse << htmlPage << "\r\n";

    logger << TLevel::Debug << "handling the 'GET' query for the resource '" << resource;
    logger << "' finished correctly\n\n";

    return queryGetResponse.str();
}


/// @brief handles the POST query and returns the weather description 
std::string NWeather::TWeatherService::PostQueryHandler(const std::string& resource, const std::string& data) const {
    logger << TLevel::Debug << "handling the 'POST' query for the resource '" << resource << "' begun\n";

    std::string weather;
    std::ostringstream queryPostResponse;

    try {
        NRequest::TRequestManager request;
        NRequest::TCacheManager cache;
        std::string city = data.substr(data.find("=") + 1);

        if (cache.IsDataExpired(city)) {
            weather = request.GetWeather(city);
            cache.InsertOrUpdateData(city, weather);
                    
        } else {
            weather = cache.GetData(city);
        }

        weather = ConvertWeatherToHtml(weather);

    } catch (NRequest::TRequestException& excp) {
        if (excp.GetErrorId() == 405 || excp.GetErrorId() == 401) {
            logger << TLevel::Error << "~ PostQueryHandler() error: the wrong data was got: " << excp.what() << "\n\n";

            queryPostResponse << ErrorCodesAndResponses.at("406");
            queryPostResponse << "<html><head><title>406 Not Acceptable</title></head><body><h1>406 Not Acceptable</h1>"
                "<p>The server couldn't find the weather for this city. Check it and try again.</p></body></html>\r\n";
            
            return queryPostResponse.str();

        } else {
            logger << TLevel::Fatal << "~ PostQueryHandler() error: tthe error that don't let handle the query was generated: ";
            logger << excp.what() << "\n\n";

            queryPostResponse << ErrorCodesAndResponses.at("502");
            queryPostResponse << "<html><head><title>502 Bad Gateway</title></head><body><h1>502 Bad Gateway</h1>"
                "<p>The server's error was generated. Try again later.</p></body></html>\r\n";
        }

        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
            criticalError = true;
        }

        return queryPostResponse.str();

    } catch (...) {
        logger << TLevel::Fatal << "~ PostQueryHandler() error: the unpredictable error was generated\n\n";
        criticalError = true;

        queryPostResponse << ErrorCodesAndResponses.at("502");
        queryPostResponse << "<html><head><title>502 Bad Gateway</title></head><body><h1>502 Bad Gateway</h1>"
                "<p>The server's error was generated. Try again later.</p></body></html>\r\n";

        return queryPostResponse.str();
    }

    queryPostResponse << "HTTP/1.1 200 OK\r\n";
    queryPostResponse << "Cache-Control: no-store\r\n";
    queryPostResponse << "Server: 127.0.0.1:8080\r\n";
    queryPostResponse << "Content-Length: " << weather.size() << "\r\n";
    queryPostResponse << "Connection: Closed\r\n";
    queryPostResponse << "Content-Type: text/html; charset=ascii\r\n\r\n";
    queryPostResponse << weather << "\r\n";

    logger << TLevel::Debug << "handling the 'POST' query for the resource '" << resource;
    logger << "' was finished correctly\n\n";

    return queryPostResponse.str();
}


/// @brief forms the answer for the client 
std::string NWeather::TWeatherService::FormResponse(const std::string& query) const {
    logger << TLevel::Debug << "forming the response begun\n";

    TQueryParams queryParams = ParseQuery(query);

    if (queryParams.Method == "GET") {
        return GetQueryHandler(queryParams.Resource);

    } else if (queryParams.Method == "POST") {
        return PostQueryHandler(queryParams.Resource, queryParams.Data);

    } else {
        logger << TLevel::Warning << "using of the method '" << queryParams.Method;
        logger << "' IS NOT allowed\n\n";

        std::ostringstream queryResponse;

        queryResponse << ErrorCodesAndResponses.at("405");
        queryResponse << "<html><head><title>405 Method Not Allowed</title></head><body><h1>Method Not Allowed</h1>"
                "<p>The method is not allowed. Try again with others.</p></body></html>\r\n";
        
        return queryResponse.str();
    }
}


/// @brief serve the client: gets the request from the client and returns the answer
void NWeather::TWeatherService::ServeTheClient(std::unique_ptr<TSocket> clientSock) const {
    logger << TLevel::Debug << "the serving new client begun\n";

    try {
        std::string buffer(4096, '\0');
        clientSock->read_some(boost::asio::buffer(buffer));

        std::string response = FormResponse(buffer);
        clientSock->write_some(boost::asio::buffer(response));

        clientSock->close();

    } catch (boost::system::system_error& excp) {
        logger << TLevel::Fatal << "~ ServeTheClient() error: error while serving the client:\n";
        logger << "\t" << excp.what() << "\n\n";
        criticalError = true;
        return;

    } catch (...) {
        logger << TLevel::Fatal << "~ ServeTheClient() error: unpredictable error was generated\n\n";
        criticalError = true;
        return;
    }
 
    logger << TLevel::Debug << "the client was served correctly\n\n";
}


/// @brief main service function
void NWeather::TWeatherService::RunService() const {
    logger << TLevel::Info << "SERVICE WAS STARTED\n-------------------\n\n";
    
    try {
        boost::asio::io_service weatherService;
        boost::asio::ip::tcp::endpoint weatherServiceEp(boost::asio::ip::address::from_string("127.0.0.1"), 80);
        boost::asio::ip::tcp::acceptor weatherAcceptor(weatherService, weatherServiceEp);

        while (true) {
            std::unique_ptr<TSocket> newClientSock(new TSocket (weatherService));

            try {
                weatherAcceptor.accept(*newClientSock);
            
                //std::thread(ServeTheClient, std::move(newClientSock)).detach();
                ServeTheClient(std::move(newClientSock));

                if (criticalError) {
                    logger << TLevel::Fatal << "the critical error was generated\n\n";
                    logger << "-------------------\nSERVICE IS DOWN\n";
                    break;
                }

            } catch (boost::system::system_error& excp) {
                std::cerr << "Error\n";
                logger << TLevel::Fatal << "the acceptor generated the fatal error: " << excp.what() << "\n\n";
                logger << "-------------------\nSERVICE IS DOWN\n";
                break;
            }
        }

    } catch (boost::system::system_error& excp) {
        std::cerr << "Error";
        logger << TLevel::Fatal << "~ RunService() error: the fatal error was generated: " << excp.what() << "\n\n";
        logger << "-------------------\n";
        logger << TLevel::Info << "SERVICE IS DOWN\n";
    }
}
