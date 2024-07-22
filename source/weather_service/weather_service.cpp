#include "./include/weather_service.h"

/// @brief parse the query from the client to realize what do the server need to do 
NWeather::TQueryParams NWeather::TWeatherService::ParseQuery(const std::string& query) const {
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

    queryParam.Data = query.substr(query.find("\r\n\r\n") + 4);

    return queryParam;
}


/// @brief forms the answer for the client 
std::string NWeather::TWeatherService::FormAnswer(const std::string& query) const {
    TQueryParams queryParams = ParseQuery(query);


    return "";
}


/// @brief serve the client: gets the request from the client and returns the answer
void NWeather::TWeatherService::ServeTheClient(std::unique_ptr<TSocket> clientSock) const {
    NRequest::TCacheManager cache;
    NRequest::TRequestManager request;
    std::string buffer(4096, '\0');

    clientSock->read_some(boost::asio::buffer(buffer));
    std::string answer = FormAnswer(buffer);

    clientSock->close();
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