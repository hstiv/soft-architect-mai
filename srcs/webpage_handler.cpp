#include "../includes/headers.hpp"

class WebPageHandler : public HTTPRequestHandler
{
public:
    WebPageHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        std::ostream &ostr = response.send(); // выходной поток
        string url = request.getURI();
        string path2file = TEMPLATES_PATH + url;
        FILE *fp = fopen(path2file.c_str(), "rb");
        if(!fp)
        {
            cout << "path " << path2file << " not found" << endl;
            ostr << ERROR_404;
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        }
        else
        {
            vector<char> buf(400); // читаем с помощью буфера
            int len;
            while(len = fread(buf.data(), sizeof(char), buf.size() - 1, fp))
            {
                buf[len] = '\0';
                ostr << buf.data();
            }
            fclose(fp);
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        }
    }

private:
    std::string _format;
};