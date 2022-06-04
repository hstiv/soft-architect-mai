#include "../includes/headers.hpp"

bool running = true;

int main(int argc, char *argv[])
{
    try
    {
        // Stop processing on SIGINT
        signal(SIGINT, [](int)
               { running = false; });

        // Construct the configuration
        cppkafka::Configuration config = {
            {"metadata.broker.list", Config::queue},
            {"group.id", STR(Config::group_id)},
            // Disable auto commit
            {"enable.auto.commit", false}};

        // Create the consumer
        cppkafka::Consumer consumer(config);

        // Print the assigned partitions on assignment
        consumer.set_assignment_callback([](const cppkafka::TopicPartitionList &partitions)
                                         { cout << "Got assigned: " << partitions << endl; });

        // Print the revoked partitions on revocation
        consumer.set_revocation_callback([](const cppkafka::TopicPartitionList &partitions)
                                         { cout << "Got revoked: " << partitions << endl; });

        // Subscribe to the topic
        consumer.subscribe({Config::topic});

        cout << "Consuming messages from topic " << Config::topic << endl;

        // Now read lines and write them into kafka
        while (running)
        {
            // Try to consume a message
            cppkafka::Message msg = consumer.poll();
            if (msg)
            {
                // If we managed to get a message
                if (msg.get_error())
                {
                    // Ignore EOF notifications from rdkafka
                    if (!msg.is_eof())
                    {
                        cout << "[+] Received error notification: " << msg.get_error() << endl;
                    }
                }
                else
                {
                    // Print the key (if any)
                    if (msg.get_key())
                    {
                        cout << msg.get_key() << " -> ";
                    }
                    // Print the payload
                    std::string payload = msg.get_payload();
                    cout << msg.get_payload() << endl;

                    Poco::JSON::Parser parser;
                    Person person;

                    bool success = true;
                    try
                    {
                        Poco::Dynamic::Var result = parser.parse(payload);
                        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
                        person.login = object->get("login").toString();
                        person.first_name = object->get("first_name").toString();
                        person.last_name = object->get("last_name").toString();
                        object->get("age").convert(person.age);
                    }
                    catch(...)
                    {
                        success = false;
                        cout << "Error in parse: " << payload << " to json" << endl;
                        continue;
                    }

                    auto session_ptr = unique_ptr<SqlSession>(create_SQL_session());
                    auto &session = *session_ptr;

                    /* Проверяем есть ли такой человек*/
                    SQL_HANDLE(
                        Statement SELECT(session);
                        SELECT << "SELECT login FROM Person WHERE login=?",
                            Keywords::use(person.login),
                            Keywords::range(0, 1);
                        SELECT.execute();

                        Poco::Data::RecordSet rs(SELECT);
                        if (rs.moveFirst()) success = false;
                    )

                    if(success)
                    {
                        // добавляем в базу
                        SQL_HANDLE(
                            Statement INSERT(session);
                            INSERT << "INSERT INTO Person (login, first_name, last_name, age)"
                                      "VALUES (?, ?, ?, ?)",
                                Keywords::use(person.login),
                                Keywords::use(person.first_name),
                                Keywords::use(person.last_name),
                                Keywords::use(person.age),
                                Keywords::range(0, 1);
                            INSERT.execute();
                        )
                    }

                    // Now commit the message
                    consumer.commit(msg);
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << endl;
    }

    return 1;
}