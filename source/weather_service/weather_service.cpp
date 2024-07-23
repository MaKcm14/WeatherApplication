#include "./include/weather_service.h"

TLogger logger;

/// @brief parses the query from the client to realize what do the server need to do 
NWeather::TQueryParams NWeather::TWeatherService::ParseQuery(const std::string& query) const {
    logger << TLevel::Debug << "parsing the query from the client begun\n";

    TQueryParams queryParam;

    queryParam.Method = query.find("GET") < query.size() ? "GET" : "POST";

    queryParam.Resource = query.substr(query.find(" ") + 1, query.find(" ", query.find(" ") + 1) - 
        query.find(" ") - 1);

    if (query.find("Connection: ") < query.size()) {
        queryParam.Connection = query.substr(query.find("Connection: ") + 12, 
            query.find("\n", query.find("Connection:")) - query.find("Connection: ") - 13);
    }

    if (query.find("Content-Length: ") < query.size()) {
        queryParam.ContentLength = query.substr(query.find("Content-Length: ") + 16,
            query.find("\n", query.find("Content-Length: ")) - query.find("Content-Length") - 17);
    }

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
    std::ifstream resourceStream(std::string("../source/weather_service/resource") + resource + ".html");
    std::ostringstream queryGetResponse;

    if (!resourceStream.is_open()) {
        logger << TLevel::Fatal << "the file '" << (resource.substr(1) + ".txt") << "' couldn't been opened (or it doesn't exist)\n\n";
        
        queryGetResponse << "HTTP/1.1 404 Not Found\r\n";
        queryGetResponse << "Cache-Control: no-store\r\n";
        queryGetResponse << "Server: 127.0.0.1:8080\r\n";
        queryGetResponse << "Content-Length: 145\r\n";
        queryGetResponse << "Connection: Closed\r\n";
        queryGetResponse << "Content-Type: text/html; charset=ascii\r\n\r\n";
        queryGetResponse << "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1>"
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


std::string NWeather::TWeatherService::PostQueryHandler(const std::string& resource, const std::string& data) const {
    logger << TLevel::Debug << "handling the 'POST' query for the resource '" << resource << "' begun\n";

    std::string weather;
    NRequest::TRequestManager request;
    NRequest::TCacheManager cache;
    std::ostringstream queryPostResponse;

    for (bool noThrow = false; !noThrow; ) {
        try {
            std::string city = data.substr(data.find("=") + 1);

            if (cache.IsDataExpired(city)) {
                weather = request.GetWeather(city);
                cache.InsertOrUpdateData(city, weather);
                    
            } else {
                weather = cache.GetData(city);
            }

            noThrow = true;

        } catch (NRequest::TRequestException& excp) {
            if (excp.GetErrorId() == 405 || excp.GetErrorId() == 401) {
                logger << TLevel::Error << "the wrong data was got: " << excp.what() << "\n\n";

                queryPostResponse << "HTTP/1.1 406\r\n";
                queryPostResponse << "Cache-Control: no-store\r\n";
                queryPostResponse << "Server: 127.0.0.1:8080\r\n";
                queryPostResponse << "Content-Length: 183\r\n";
                queryPostResponse << "Connection: Closed\r\n";
                queryPostResponse << "Content-Type: text/html; charset=ascii\r\n\r\n";
                queryPostResponse << "<html><head><title>406 Not Acceptable</title></head><body><h1>406 Not Acceptable</h1>"
                    "<p>The server couldn't find the weather for this city. Check it and try again.</p></body></html>\r\n";
                
                return queryPostResponse.str();

            } else if (excp.GetErrorId() >= 400 || excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
                logger << TLevel::Fatal << "the error that don't let handle the query was generated: ";
                logger << excp.what() << "\n\n";

                queryPostResponse << "HTTP/1.1 502\r\n";
                queryPostResponse << "Cache-Control: no-store\r\n";
                queryPostResponse << "Server: 127.0.0.1:8080\r\n";
                queryPostResponse << "Content-Length: 152\r\n";
                queryPostResponse << "Connection: Closed\r\n";
                queryPostResponse << "Content-Type: text/html; charset=ascii\r\n\r\n";
                queryPostResponse << "<html><head><title>502 Bad Gateway</title></head><body><h1>502 Bad Gateway</h1>"
                    "<p>The server's error was generated. Try again later.</p></body></html>\r\n";

                return queryPostResponse.str();
                
            } else {
                logger << TLevel::Error << "the error was generated: " << excp.what() << "\n\n";
            }

        } catch (...) {
            logger << TLevel::Fatal << "the unpredictable error was generated\n\n";

            queryPostResponse << "HTTP/1.1 502\r\n";
            queryPostResponse << "Cache-Control: no-store\r\n";
            queryPostResponse << "Server: 127.0.0.1:8080\r\n";
            queryPostResponse << "Content-Length: 152\r\n";
            queryPostResponse << "Connection: Closed\r\n";
            queryPostResponse << "Content-Type: text/html; charset=ascii\r\n\r\n";
            queryPostResponse << "<html><head><title>502 Bad Gateway</title></head><body><h1>502 Bad Gateway</h1>"
                "<p>The server's error was generated. Try again later.</p></body></html>\r\n";

            return queryPostResponse.str(); 
        }
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
    TQueryParams queryParams = ParseQuery(query);

    if (queryParams.Method == "GET") {
        return GetQueryHandler(queryParams.Resource);

    } else if (queryParams.Method == "POST") {
        return PostQueryHandler(queryParams.Resource, queryParams.Data);

    } else {
        logger << TLevel::Error << "the using of the method '" << queryParams.Method;
        logger << "' IS NOT allowed\n\n";
        std::ostringstream queryResponse;

        queryResponse << "HTTP/1.1 405\r\n";
        queryResponse << "Cache-Control: no-store\r\n";
        queryResponse << "Server: 127.0.0.1:8080\r\n";
        queryResponse << "Content-Length: 161\r\n";
        queryResponse << "Connection: Closed\r\n";
        queryResponse << "Content-Type: text/html; charset=ascii\r\n\r\n";
        queryResponse << "<html><head><title>405 Method Not Allowed</title></head><body><h1>Method Not Allowed</h1>"
                "<p>The method is not allowed. Try again with others.</p></body></html>\r\n";
        
        return queryResponse.str();
    }
}


/// @brief serve the client: gets the request from the client and returns the answer
void NWeather::TWeatherService::ServeTheClient(std::unique_ptr<TSocket> clientSock) const {
    std::string buffer(4096, '\0');

    clientSock->read_some(boost::asio::buffer(buffer));
    std::string response = FormResponse(buffer);

    clientSock->write_some(boost::asio::buffer(response));
}


/// @brief main service function
void NWeather::TWeatherService::RunService() const {
    boost::asio::io_service weatherService;
    boost::asio::ip::tcp::endpoint weatherServiceEp(boost::asio::ip::address::from_string("127.0.0.1"), 8080);
    boost::asio::ip::tcp::acceptor weatherAcceptor(weatherService, weatherServiceEp);

    while (true) {
        std::unique_ptr<TSocket> newClientSock(new TSocket (weatherService));
        /// TODO: add the multithreading here for work with clients
        weatherAcceptor.accept(*newClientSock);
        ServeTheClient(std::move(newClientSock));
    }
}