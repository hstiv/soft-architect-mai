#include "../includes/headers.hpp"

bool starts_with(const string &str, const string &prefix)
{
    if(prefix.size() > str.size())
        return false;

    int i;
    for(i = 0; i < prefix.size(); i++)
    {
        if(str[i] != prefix[i])
        {
            return false;
        }
    }
    return true;
}

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
