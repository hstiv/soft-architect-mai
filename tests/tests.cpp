#include "../includes/headers.hpp"

/*
 * Before tests, run build/server.exe
*/

TEST(test_create, basic_test_set) // создание базы
{
    testing::internal::CaptureStdout();

    auto session_ptr = create_SQL_session();
    Poco::Data::Session &session = *session_ptr;
    SQL_HANDLE(
        Statement DROP(session);
        DROP << "DROP TABLE IF EXISTS Person;"; 
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
                  ");";
        CREATE.execute();
    )
    delete session_ptr;
    ASSERT_TRUE(testing::internal::GetCapturedStdout() == "");
}

TEST(test_add, basic_test_set) // добавление записей
{
    vector<Person> persons = {{string("hstiv"), string("Cleopatry"), string("Brusnichiy"), 22},
                              {string("duradura"), string("Dora"), string("Dura"), 21}};

    int i;
    for(i = 0; i < persons.size(); i++) // последовательно создаём запросы на добавление
    {
        Poco::Net::SocketAddress sa(Config::ip, Config::port);
        Poco::Net::StreamSocket socket(sa);

        Poco::Net::SocketStream str(socket);
        str << "POST /person HTTP/1.1\n" <<
               "content-type: application/url-encoded\n\n" <<
               "add=True&login=" << persons[i].login << 
               "&first_name=" << persons[i].first_name <<
               "&last_name=" << persons[i].last_name <<
               "&age=" << persons[i].age << "\n";
        str.flush();
    }
    
    sleep(1); // ждём на всякий случай

    testing::internal::CaptureStdout(); // лезем в базу и проверяем, что записи добавились
    auto session_ptr = create_SQL_session();
    Poco::Data::Session &session = *session_ptr;

    for(i = 0; i < persons.size(); i++)
    {
        Person person;
        SQL_HANDLE(
            Statement SELECT(session);
            SELECT << "SELECT login, first_name, last_name, age FROM Person WHERE login=? AND first_name=? AND last_name=? AND age=?;",
                Keywords::into(person.login),
                Keywords::into(person.first_name),
                Keywords::into(person.last_name),
                Keywords::into(person.age),
                Keywords::use(persons[i].login),
                Keywords::use(persons[i].first_name),
                Keywords::use(persons[i].last_name),
                Keywords::use(persons[i].age),
                Keywords::range(0, 1);
            SELECT.execute();

            Poco::Data::RecordSet rs(SELECT);
            ASSERT_TRUE(rs.moveFirst());
        )
    }
    delete session_ptr; // удаляем сессию
    ASSERT_TRUE(testing::internal::GetCapturedStdout() == "");
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}