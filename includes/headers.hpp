#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <stdio.h>
#include <stdlib.h>
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

#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

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

#define TEMPLATES_PATH	"templates"
#define DESC    "usage: ./tests.exe --ip=<machine_ip>\n \
                example: ./tests.exe --ip=192.168.31.63"
#define ERROR_404 " <html lang=\"ru\"> \
                    <head><title>Web Server</title></head> \
                    <body><h1>Error 404: page not found</h1></body> \
                    </html>"
#define DEFAULT_IP "192.168.1.50"
namespace Config // глобальные переменные
{
    string host     = "127.0.0.1",
           login    = "hstiv",
           password = "112233qq",
           database = "test_db",
           ip = "";
    int port = 8080;
    int sql_port = 6033;
    int n_shards = 2;
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
                               ";password=" +  Config::password +
                               ";port=" + std::to_string(Config::sql_port);

    Poco::Data::MySQL::Connector::registerConnector();
    SqlSession *session_ptr = NULL;
    SQL_HANDLE(
        session_ptr = new Poco::Data::Session(Poco::Data::SessionFactory::instance().create(
            Poco::Data::MySQL::Connector::KEY, connection_string));
    )
    return session_ptr;
}

int get_shard_id(const string &login)
{
    return std::hash<string>{}(login) % Config::n_shards;
}

void WAIT_ALL_THREADS(vector<thread *> &vec) // закрытие потоков
{
    int i;
    for(i = 0; i < vec.size(); i++) 
    {
        vec[i]->join(); 
        delete vec[i];
    }
}

void argv2map(int argc, char *argv[], std::map<string, string> &args, const string &desc)
{
    int i;
    for(i = 1; i < argc; i++)
    {
        int j;
        string arg(argv[i]);
        j = std::find(arg.begin(), arg.end(), '=') - arg.begin();
        if(j == arg.size())
        {
            cout << "ERROR in argc[" + STR(i) + "]" << endl;
            cout << desc << endl;
            return;
        }
        args[arg.substr(0, j)] = arg.substr(j + 1, arg.size() - j);
    }
}

void argv2map(int argc, char *argv[], std::map<string, string> &args)
{
    string desc = "";
    argv2map(argc, argv, args, desc);
}

#endif