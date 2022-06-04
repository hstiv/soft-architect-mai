#include "../includes/headers.hpp"
#include "request_handler.cpp"
#include "webpage_handler.cpp"
#include "utils.cpp"

class HTTPRequestFactory : public HTTPRequestHandlerFactory
{
public:
    HTTPRequestFactory(const std::string &format) : _format(format)
    {
    }

    HTTPRequestHandler *createRequestHandler(const HTTPServerRequest &request)
    {
        cout << "Request " + request.getMethod() + " " +  request.getURI() + " from " +
        request.clientAddress().toString() << endl;
        if (starts_with(request.getURI(), "/person"))
        {
            return new RequestHandler(_format);
        }
        else
        {
            return new WebPageHandler(_format);
        }
    }

private:
    std::string _format;
};

class HTTPWebServer : public Poco::Util::ServerApplication
{
public:
    HTTPWebServer() : _helpRequested(false)
    {
    }

    ~HTTPWebServer()
    {
    }

protected:
    void initialize(Application &self)
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    void uninitialize()
    {
        ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet &options)
    {
        ServerApplication::defineOptions(options);
    }

    int main(const std::vector<std::string> &args)
    {
        if (!_helpRequested)
        {
            unsigned short port = (unsigned short)config().getInt("HTTPWebServer.port", Config::port);
            std::string format(
                config().getString("HTTPWebServer.format",
                                   DateTimeFormat::SORTABLE_FORMAT));

            ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", port));
            HTTPServer srv(new HTTPRequestFactory(format),
                           svs, new HTTPServerParams);
            try {
                std::cout << "Started server on " << Config::ip << ":" << STR(port) << std::endl;
                srv.start();
            }
            catch (...)
            {
                cout << "ERROR" << endl;
                cout << DESC << endl;
                exit(-1);
            }
            

            waitForTerminationRequest();
            srv.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};