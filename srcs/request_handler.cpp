#include "../includes/headers.hpp"

class RequestHandler : public HTTPRequestHandler
{
public:
    RequestHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        Poco::Net::HTMLForm form(request, request.stream());
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        std::ostream &ostr = response.send();
        string method = request.getMethod();
        
        if (method == "GET" && form.has("login"))
        {
            std::string req_login = form.get("login"); // получаем логин

            auto session_ptr = unique_ptr<SqlSession>(create_SQL_session());
            auto &session = *session_ptr;
        
            Person person;
            bool success = true;

            SQL_HANDLE(
                Statement SELECT(session);
                SELECT << "SELECT login, first_name, last_name, age FROM Person WHERE login=?" <<
                          " -- sharding:" << STR(get_shard_id(req_login)),
                    Keywords::into(person.login),
                    Keywords::into(person.first_name),
                    Keywords::into(person.last_name),
                    Keywords::into(person.age),
                    Keywords::use(req_login),
                    Keywords::range(0, 1);
                SELECT.execute();

                Poco::Data::RecordSet rs(SELECT);
                if (!rs.moveFirst()) throw std::logic_error("not found");
            )
            catch(std::logic_error &e) // пользователь не найден
            {
                cout << req_login << " not found" << endl;
                success = false;
            }

            try
            {
                Poco::JSON::Array arr;
                
                if(success)
                {
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("login", person.login);
                    root->set("first_name", person.first_name);
                    root->set("last_name", person.last_name);
                    root->set("age", person.age);
                    arr.add(root);
                }
                
                Poco::JSON::Stringifier::stringify(arr, ostr);
            }
            catch (...)
            {
                std::cout << "exception" << std::endl;
            }
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        }
        else if(method == "GET" && form.has("first_name") && form.has("last_name"))
        {
            string first_name_mask = form.get("first_name"),
                   last_name_mask =  form.get("last_name");

            /* Функция, которую обрабатывает один поток */
            auto send_request = [](int shard_id,
                                   string first_name_mask, string last_name_mask, 
                                   vector<Person> *result) -> void 
            {
                auto session_ptr = unique_ptr<SqlSession>(create_SQL_session());
                Poco::Data::Session &session = *session_ptr;
            
                Person person;

                SQL_HANDLE(
                    Statement SELECT(session);
                    SELECT << "SELECT login, first_name, last_name, age FROM Person WHERE first_name LIKE ? AND last_name LIKE ?" <<
                              " -- sharding:" << STR(shard_id),
                        Keywords::into(person.login),
                        Keywords::into(person.first_name),
                        Keywords::into(person.last_name),
                        Keywords::into(person.age),
                        Keywords::use(first_name_mask),
                        Keywords::use(last_name_mask),
                        Keywords::range(0, 1);

                    while(!SELECT.done())
                    {
                        if(SELECT.execute())
                            result->push_back(person);
                    }
                )
            };

            vector<vector<Person> *> shards_result(Config::n_shards);
            vector<thread *> vec_threads(Config::n_shards);
            int i;
            for(i = 0; i < Config::n_shards; i++) // запускаем процессы ко всем шардам, результат в shards_result
            {
                shards_result[i] = new vector<Person>(0); // заказ вектора
                vec_threads[i] = new thread(send_request, i, 
                                            first_name_mask, last_name_mask,
                                            shards_result[i]);
            }
            WAIT_ALL_THREADS(vec_threads);
            
            try // отправляем результаты пользователю
            {
                Poco::JSON::Array arr;
                int i, j;
                for(i = 0; i < shards_result.size(); i++) // цикл по шардам
                    for(j = 0; j < shards_result[i]->size(); j++) // цикл по ответам внутри шарда
                    {
                        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                        root->set("login", shards_result[i]->at(j).login);
                        root->set("first_name", shards_result[i]->at(j).first_name);
                        root->set("last_name", shards_result[i]->at(j).last_name);
                        root->set("age", shards_result[i]->at(j).age);
                        arr.add(root);
                    }
                Poco::JSON::Stringifier::stringify(arr, ostr);
            }
            catch (...)
            {
                std::cout << "exception" << std::endl;
            }
            for(i = 0; i < shards_result.size(); i++) // очистка памяти
                delete shards_result[i];
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        }
        else if(method == "POST" && form.has("login") && form.has("last_name") && form.has("first_name") && form.has("age"))
        {
            string login = form.get("login"),
                   last_name =  form.get("last_name"),
                   first_name = form.get("first_name");
            int age = atoi(form.get("age").c_str());

            auto session_ptr = unique_ptr<SqlSession>(create_SQL_session());
            auto &session = *session_ptr;

            bool success = true;

            /* Проверяем есть ли такой человек*/
            SQL_HANDLE(
                Statement SELECT(session);
                SELECT << "SELECT login FROM Person WHERE login=?" << " -- sharding:" << STR(get_shard_id(login)),
                    Keywords::use(login),
                    Keywords::range(0, 1);
                SELECT.execute();

                Poco::Data::RecordSet rs(SELECT);
                if (rs.moveFirst()) success = false;
            )
            
            if(success) // добавляем
            {
                SQL_HANDLE(
                    Statement INSERT(session);
                    INSERT << "INSERT INTO Person (login, first_name, last_name, age)"
                              "VALUES (?, ?, ?, ?)" << " -- sharding:" << STR(get_shard_id(login)),
                        Keywords::use(login),
                        Keywords::use(first_name),
                        Keywords::use(last_name),
                        Keywords::use(age),
                        Keywords::range(0, 1);
                    INSERT.execute();
                )
            }

            ostr << "<html lang=\"ru\">"
                    "<head><title>Web Server</title></head>"
                    "<body><h1>" + (success ? string("OK") : string("Already exists")) + "</h1></body>"
                    "</html>";
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        }
        else
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
        }
    }

private:
    std::string _format;
};