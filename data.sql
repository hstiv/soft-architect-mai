DROP TABLE IF EXISTS Person;

CREATE TABLE IF NOT EXISTS Person
(
    login VARCHAR(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL PRIMARY KEY,
    first_name VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_unicode_ci  NULL,
    last_name VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_unicode_ci  NOT NULL,
    age INT NULL CHECK(age >= 0)
);