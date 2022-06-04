#include "../includes/headers.hpp"
#include "request_factory.cpp"

int main(int argc, char *argv[])
{
    HTTPWebServer app;

    return app.run(1, argv); // POCO принимает свои параметры командной строки, передаём их отсутствие
}