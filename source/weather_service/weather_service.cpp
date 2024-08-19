#include "./include/weather_service.h"

TLogger logger;

namespace NRequest {
    extern nlohmann::json configJson;
}

namespace NWeather {
    std::atomic<bool> criticalError = false;
    std::mutex serviceMut;
}


/// @brief converts the weather description in the html format 
std::string NWeather::TQueryConverter::ConvertWeatherToHtml(const std::string& weather) const {
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
NWeather::TQueryHandler::TQueryParams NWeather::TQueryHandler::ParseQuery(const std::string& query) const {
    logger << TLevel::Debug << "parsing the query from the client begun\n";

    TQueryParams queryParam;

    queryParam.Method = query.substr(0, query.find(" "));

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
std::string NWeather::TGetMethod::IProcessRequest(const TQueryHandler::TQueryParams& query) const {
    logger << TLevel::Debug << "handling the 'GET' query for the resource '" << query.Resource << "' begun\n";

    std::string htmlPage;
    std::ostringstream queryGetResponse;
    std::ifstream resourceStream;

    try {
        resourceStream.open(std::string("../source/weather_service/resource") + 
            TQueryHandler::TQueryParams::ResourcesToFiles.at(query.Resource));
    
    } catch (std::exception& excp) {
        resourceStream.close();
    }

    if (!resourceStream.is_open()) {
        logger << TLevel::Warning << "the file for resource '" << query.Resource << "' wasn't opened (or it doesn't exist)\n\n";

        queryGetResponse << TQueryHandler::TQueryParams::ErrorCodesAndResponses.at("404");
        queryGetResponse << "Content-Length: " << 150 << "\r\n\r\n";
        queryGetResponse << "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1>" \
                    "<p>The requested resource was not found on this server.</p></body></html>\r\n";
        
        return queryGetResponse.str();
    }

    for (char symb; resourceStream.get(symb); ) {
        htmlPage.push_back(symb);
    }

    queryGetResponse << "HTTP/1.1 200 OK\r\n";
    queryGetResponse << "Cache-Control: no-store\r\n";
    queryGetResponse << "Server: localhost\r\n";
    queryGetResponse << "Content-Length: " << htmlPage.size() << "\r\n";
    queryGetResponse << "Connection: Close\r\n";
    queryGetResponse << "Content-Type: text/html; charset=ascii\r\n\r\n";
    queryGetResponse << htmlPage << "\r\n";

    logger << TLevel::Debug << "handling the 'GET' query for the resource '" << query.Resource;
    logger << "' finished correctly\n\n";

    return queryGetResponse.str();
}


/// @brief handles the POST query for the '/weather' resource 
std::string NWeather::TPostMethod::WeatherResourceHandler(const TQueryHandler::TQueryParams& query) const {
    std::string weather;
    std::ostringstream queryPostResponse;

    try {
        NRequest::TRequestManager request;

        logger << TLevel::Info << "set connection to the DB\n";
        NRequest::TCacheManager cache(std::make_unique<NDataBase::TPostgreSql>("127.0.0.1", "5432", 
            NRequest::configJson.at("db_password").dump()));

        std::string city = query.Data.substr(query.Data.find("=") + 1);

        if (cache.IsDataExpired(city)) {
            weather = request.GetWeatherTemplate(city);
            cache.InsertOrUpdateData(city, weather);
                    
        } else {
            weather = cache.GetData(city);
        }

        weather = Converter.ConvertWeatherToHtml(weather);

    } catch (NRequest::TRequestException& excp) {
        if (excp.GetErrorId() == 401 || excp.GetErrorId() == 404 || excp.GetErrorId() == 405) {
            logger << TLevel::Error << "~ PostQueryHandler() error: the wrong data was got: " << excp.what() << "\n\n";

            queryPostResponse << TQueryHandler::TQueryParams::ErrorCodesAndResponses.at("400");
            queryPostResponse << "Content-Length: " << 177 << "\r\n\r\n";
            queryPostResponse << "<html><head><title>400 Bad Request</title></head><body><h1>400 Bad Request</h1>"
                "<p>The server couldn't find the weather for this city. Check it and try again.</p></body></html>\r\n";
            
            return queryPostResponse.str();

        } else if (excp.GetErrorId() == 501) {
            logger << TLevel::Fatal << "~ PostQueryHandler() error: the meteo-server has problem: ";
            logger << excp.what() << "\n\n";

            queryPostResponse << TQueryHandler::TQueryParams::ErrorCodesAndResponses.at("502");
            queryPostResponse << "Content-Length: " << 155 << "\r\n\r\n";
            queryPostResponse << "<html><head><title>502 Bad Gateway</title></head><body><h1>502 Bad Gateway</h1>"
                "<p>The meteo-server has problem. Please try again later.</p></body></html>\r\n";
            
        } else {
            logger << TLevel::Error << "~ PostQueryHandler() error: internal server error was generated: ";
            logger << excp.what() << "\n\n";

            queryPostResponse << TQueryHandler::TQueryParams::ErrorCodesAndResponses.at("500");
            queryPostResponse << "Content-Length: " << 193 << "\r\n\r\n";
            queryPostResponse << "<html><head><title>500 Internal Server Error</title>"
                "</head><body><h1>500 Internal Server Error</h1>"
                "<p>There's error that was generated on the server. "
                "Please try again later.</p></body></html>\r\n";
            
        }

        if (auto errorId = excp.GetErrorId(); errorId != 401 && errorId != 404 && errorId != 405
            && errorId != 501) 
        {
            criticalError = true;
        }

        return queryPostResponse.str();

    } catch (NDataBase::TDataBaseException& excp) {
        logger << TLevel::Fatal << "~ PostQueryHandler() error: internal server error was generated (DB): ";
        logger << excp.what() << "\n\n";

        criticalError = true;

        queryPostResponse << TQueryHandler::TQueryParams::ErrorCodesAndResponses.at("500");
        queryPostResponse << "Content-Length: " << 193 << "\r\n\r\n";
        queryPostResponse << "<html><head><title>500 Internal Server Error</title>"
            "</head><body><h1>500 Internal Server Error</h1>"
            "<p>There's error that was generated on the server. "
            "Please try again later.</p></body></html>\r\n";
        
        return queryPostResponse.str();

    } catch (...) {
        logger << TLevel::Fatal << "~ PostQueryHandler() error: the unpredictable error was generated\n\n";

        criticalError = true;

        queryPostResponse << TQueryHandler::TQueryParams::ErrorCodesAndResponses.at("500");
        queryPostResponse << "Content-Length: " << 193 << "\r\n\r\n";
        queryPostResponse << "<html><head><title>500 Internal Server Error</title>"
                "</head><body><h1>500 Internal Server Error</h1>"
                "<p>There's error that was generated on the server. "
                "Please try again later.</p></body></html>\r\n";

        return queryPostResponse.str();
    }

    queryPostResponse << "HTTP/1.1 200 OK\r\n";
    queryPostResponse << "Cache-Control: no-store\r\n";
    queryPostResponse << "Server: localhost\r\n";
    queryPostResponse << "Content-Length: " << weather.size() << "\r\n";
    queryPostResponse << "Connection: Closed\r\n";
    queryPostResponse << "Content-Type: text/html; charset=ascii\r\n\r\n";
    queryPostResponse << weather << "\r\n";

    return queryPostResponse.str();
}


/// @brief handles the POST query and returns the weather description 
std::string NWeather::TPostMethod::IProcessRequest(const TQueryHandler::TQueryParams& query) const {
    logger << TLevel::Debug << "handling the 'POST' query for the resource '" << query.Resource << "' begun\n";

    std::string queryRes;

    if (query.Resource == "/weather") {
        queryRes = std::move(WeatherResourceHandler(query));
    }

    logger << TLevel::Debug << "handling the 'POST' query for the resource '" << query.Resource;
    logger << "' was finished correctly\n\n";

    return queryRes;
}


/// @brief forms the answer for the client 
std::string NWeather::TQueryHandler::FormResponse(const std::string& query) const {
    logger << TLevel::Debug << "forming the response begun\n";

    std::unique_ptr<TMethodHandler> methodHandler;
    TQueryHandler::TQueryParams queryParams = ParseQuery(query);

    if (queryParams.Method == "GET") {
        methodHandler = std::make_unique<TGetMethod>();

    } else if (queryParams.Method == "POST") {
        methodHandler = std::make_unique<TPostMethod>();

    } else {
        logger << TLevel::Warning << "using of the method '" << queryParams.Method;
        logger << "' IS NOT allowed\n\n";

        std::ostringstream queryResponse;

        queryResponse << TQueryHandler::TQueryParams::ErrorCodesAndResponses.at("405");
        queryResponse << "Content-Length: " << 161 << "\r\n\r\n";
        queryResponse << "<html><head><title>405 Method Not Allowed</title></head><body><h1>Method Not Allowed</h1>"
                "<p>The method is not allowed. Try again with others.</p></body></html>\r\n";
        
        return queryResponse.str();
    }

    return methodHandler->IProcessRequest(queryParams);
}


/// @brief serve the client: gets the request from the client and returns the answer
void NWeather::TWeatherService::ServeTheClient(std::unique_ptr<TTcpSocket> clientSock) const {
    logger << TLevel::Debug << "the serving new client begun\n";

    try {
        std::string buffer(4096, '\0');
        clientSock->read_some(boost::asio::buffer(buffer));

        std::string response = QueryHandler.FormResponse(buffer);
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


void NWeather::TWeatherService::ThreadServeClient(std::unique_ptr<TTcpSocket> clientSock) {
    NWeather::TWeatherService service;
    service.ServeTheClient(std::move(clientSock));
}


/// @brief main service function
void NWeather::TWeatherService::RunService() {
    logger << TLevel::Info << "SERVICE WAS STARTED\n-------------------\n\n";
    
    try {
        NRequest::ConfigureRequestService();

        boost::asio::io_service weatherService;
        boost::asio::ip::tcp::endpoint weatherServiceEp(boost::asio::ip::address::from_string("127.0.0.1"), 80);
        boost::asio::ip::tcp::acceptor weatherAcceptor(weatherService, weatherServiceEp);

        ///DEBUG: make while (true) loop after adding the multithreading
        for (size_t i = 0; i != 50; ++i) {
            std::unique_ptr<TTcpSocket> newClientSock = std::make_unique<TTcpSocket>(weatherService);

            try {
                weatherAcceptor.accept(*newClientSock);
                //std::thread(ThreadServeClient, std::move(newClientSock)).detach();
                ThreadServeClient(std::move(newClientSock));

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

            } catch (...) {
                std::cerr << "Error\n";
                logger << TLevel::Fatal << "the unpredictable error was generated\n\n";
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
