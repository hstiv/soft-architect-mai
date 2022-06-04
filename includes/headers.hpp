#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <exception>
#include <memory>
#include <functional>
#include <thread>
#include <sstream>
#include <map> 
#include <algorithm>

// для sleep
#ifdef _WIN32
#include <Windows.h> 
#else
#include <unistd.h>
#endif

#include "Poco/Thread.h"
#include "Poco/Runnable.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/Statement.h>

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/HTMLForm.h"

#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/StreamCopier.h"

#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"

#include "Poco/Dynamic/Var.h"

#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

#include <ignite/thin/ignite_client.h>
#include <ignite/thin/ignite_client_configuration.h>
#include <ignite/thin/cache/cache_peek_mode.h>

using std::cout;
using std::endl;
using std::string;
using std::wstring;
using std::vector;
using std::unique_ptr;
using std::thread;
#define STR std::to_string

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;

using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#define Keywords Poco::Data::Keywords
using Poco::Data::Statement;
using SqlSession = Poco::Data::Session;

#define          DESC   "app.exe --cache_servers=<cache server [address]:[post]>"
#define     ERROR_404   "<html lang=\"ru\"> \
                        <head><title>Web Server</title></head> \
                        <body><h1>Error 404: page not found</h1></body> \
                        </html>"
#define TEMPLATES_PATH	"templates"
namespace Config // глобальные переменные
{
    string host     = "localhost",
           login    = "stud",
           password = "stud",
           database = "persons_db",
           ip = "192.168.1.50";
    int port = 8080;
    string cache_servers = "127.0.0.1:10800,127.0.0.1:10900";
}

#define SQL_HANDLE(...)                                   \
try                                                       \
{                                                         \
    __VA_ARGS__                                           \
}                                                         \
catch (Poco::Data::MySQL::ConnectionException &e)         \
{                                                         \
    cout << "connection ERROR:" << e.what() << endl;      \
}                                                         \
catch (Poco::Data::MySQL::StatementException &e)          \
{                                                         \
    cout << "statement ERROR:" << e.what() << endl;       \
}

struct Person
{   
    string login;
    string first_name;
    string last_name;
    int age;
};

SqlSession *create_SQL_session()  // Создаём сессию с базой данных
{
    string connection_string = "host=" + Config::host +           
                               ";user=" +  Config::login + 
                               ";db=" +  Config::database + 
                               ";password=" +  Config::password;

    Poco::Data::MySQL::Connector::registerConnector();
    SqlSession *session_ptr = NULL;
    SQL_HANDLE(
        session_ptr = new Poco::Data::Session(Poco::Data::SessionFactory::instance().create(
            Poco::Data::MySQL::Connector::KEY, connection_string));
    )
    return session_ptr;
}

/* =================      Кеш      ================= */

namespace Cache
{
    static ignite::thin::IgniteClient _client;
    static ignite::thin::cache::CacheClient<string, string> _cache;

    void init()
    {
        ignite::thin::IgniteClientConfiguration cfg;
        cfg.SetEndPoints(Config::cache_servers);
        cfg.SetPartitionAwareness(true);
        try
        {
            _client = ignite::thin::IgniteClient::Start(cfg);
            _cache = _client.GetOrCreateCache<string, string>("persons");
        }
        catch (ignite::IgniteError* err)
        {
            cout << "error:" << err->what() << endl;
            throw;
        }
    }

    void put(string &login, string &val)
    {
        _cache.Put(login, val);
    }

    void remove(string &login)
    {
        _cache.Remove(login);
    }

    int size()
    {
        return _cache.GetSize(ignite::thin::cache::CachePeekMode::ALL);
    }

    bool get(string &login, string &val) // val - json-строка 
    {
        try
        {
            val = _cache.Get(login);
            return val != "";
        }
        catch(...)
        {
            val = "";
            return false;
        }
    }

    void remove_all()
    {
        _cache.RemoveAll();;
    }

}

#endif