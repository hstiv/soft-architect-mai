#include "../includes/headers.hpp"

/*
 * !!Run server.exe before!!
*/

void recreate_table(int shard_id)
{
    auto session_ptr = unique_ptr<SqlSession>(create_SQL_session());
    auto &session = *session_ptr;

    SQL_HANDLE(
        Statement DROP(session);
        DROP << "DROP TABLE IF EXISTS Person -- sharding:" + std::to_string(shard_id); 
        DROP.execute();
    )

    SQL_HANDLE(
        Statement CREATE(session);
        CREATE << "CREATE TABLE IF NOT EXISTS Person"
                  "("
                  "    login VARCHAR(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL PRIMARY KEY,"
                  "    first_name VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_unicode_ci  NULL,"
                  "    last_name VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_unicode_ci  NOT NULL,"
                  "    age INT NULL CHECK(age >= 0)"
                  ") -- sharding:" << STR(shard_id);
        CREATE.execute();
    )
}

void add_person(Person person)
{
    Poco::Net::SocketAddress sa(Config::ip, Config::port);
    Poco::Net::StreamSocket socket(sa);

    Poco::Net::SocketStream str(socket);
    str << "POST /person HTTP/1.1\n" <<
            "content-type: application/url-encoded\n\n" <<
            "login=" << person.login << 
            "&first_name=" << person.first_name <<
            "&last_name=" << person.last_name <<
            "&age=" << person.age << "\n";
    str.flush();
    socket.shutdownSend();
}

void check_person(Person person, int *ans)
{
    *ans = 1;
    Poco::Net::SocketAddress sa(Config::ip, Config::port);
    Poco::Net::StreamSocket socket(sa);

    Poco::Net::SocketStream str(socket);
    str << "GET /person?login=" << person.login << "\nHTTP/1.1\n";
            
    str.flush();
    socket.shutdownSend();

    std::stringstream ss;
    Poco::StreamCopier::copyStream(str, ss);

    vector<char> buf(256);
    string json;
    
    while(ss) // перевод в строку json
    {
        ss.getline(buf.data(), buf.size(), '\n');
        json += buf.data();
    }
    string sage = STR(person.age);
    if(std::search(json.begin(), json.end(), person.login.begin(), person.login.end()) == json.end() ||
       std::search(json.begin(), json.end(), person.first_name.begin(), person.first_name.end()) == json.end() ||
       std::search(json.begin(), json.end(), person.last_name.begin(), person.last_name.end()) == json.end() ||
       std::search(json.begin(), json.end(), sage.begin(), sage.end()) == json.end())
    {
        *ans = 0;
    }
}

TEST(test_create, basic_test_set) // создание базы
{
    testing::internal::CaptureStdout();

    vector<thread *> vec_threads(Config::n_shards);
    int i;

    for(i = 0; i < vec_threads.size(); i++) // запуск потоков
    {
        vec_threads[i] = new thread(recreate_table, i);
    }
    WAIT_ALL_THREADS(vec_threads);

    ASSERT_TRUE(testing::internal::GetCapturedStdout() == "");
}

TEST(test_add, basic_test_set) // добавление записей с последующей проверкой
{
    vector<Person> persons = {{string("hstiv"), string("Hallie"), string("Stiv"), 22},
                              {string("doradura"), string("Dora"), string("Dura"), 21},
                              {string("vano"), string("Momonosuke"), string("Doppo"), 42}};

    vector<thread *> vec_threads(persons.size()); // каждая запись в своём потоке, благо их не много в целях теста
    int i;
    testing::internal::CaptureStdout();

    for(i = 0; i < vec_threads.size(); i++) // параллельно создаём запросы на добавление
    {
        vec_threads[i] = new thread(add_person, persons[i]);
    }
    WAIT_ALL_THREADS(vec_threads);
    
    sleep(4); // ждём на всякий случай
    vector<int> res(persons.size()); // результаты запросов

    for(i = 0; i < vec_threads.size(); i++) // параллельно создаём запросы на поиск
    {
        vec_threads[i] = new thread(check_person, persons[i], res.data() + i);
    }
    WAIT_ALL_THREADS(vec_threads);

    ASSERT_TRUE(testing::internal::GetCapturedStdout() == ""); // проверяем ошибки
    for(i = 0; i < res.size(); i++) // проверяем ответы
    {
        ASSERT_TRUE(res[i]);
    }
}

int main(int argc, char *argv[])
{
    std::map<string, string> args; // разбор входных параметров
    argv2map(argc, argv, args, DESC);

    if(args.find("--ip") == args.end())
    {
        cout << "WARNING: not find --ip arg, DEFAULT_IP=192.168.1.50 will be used" << endl;
        Config::ip = DEFAULT_IP;
        cout << DESC << endl;
        return 0;
    }
    else
        Config::ip = args["--ip"];

    try
    {
        testing::InitGoogleTest(&argc, argv);
    }
    catch(...)
    {
        cout << "ERROR" << endl;
        cout << DESC << endl;
        return(-1);
    }

    return RUN_ALL_TESTS();
}