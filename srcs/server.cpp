#include "../includes/headers.hpp"
#include "request_factory.cpp"

int prepare_db()
{
    try
    {
        vector<thread *> vec_threads(Config::n_shards);

        for(int i = 0; i < vec_threads.size(); i++) // запуск потоков
        {
            vec_threads[i] = new thread(recreate_table, i);
        }
        WAIT_ALL_THREADS(vec_threads);
    }
    catch(...)
    {
        cout << "ERROR: Could not create table Person" << endl;
        return (1);
    }
    
    return (0);
}

int main(int argc, char *argv[])
{
    HTTPWebServer app;

    if (prepare_db())
        return (-1);

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