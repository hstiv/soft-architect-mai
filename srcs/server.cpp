#include "../includes/headers.hpp"
#include "request_factory.cpp"

int main(int argc, char *argv[])
{
    HTTPWebServer app;

    std::map<string, string> args; // разбор входных параметров
    argv2map(argc, argv, args, DESC);

    if(args.find("--ip") == args.end())
    {
        cout << "WARNING: not find --ip arg, DEFAULT_IP=192.168.1.50 will be used" << endl;
        Config::ip = DEFAULT_IP;
    } 
    else
        Config::ip = args["--ip"];

    return app.run(1, argv); // POCO принимает свои параметры командной строки, передаём их отсутствие
}