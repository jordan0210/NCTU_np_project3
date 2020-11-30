#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include <fstream>
#include <string.h>

using boost::asio::ip::tcp;
using namespace std;

boost::asio::io_context io_context;

class session
    : public std::enable_shared_from_this<session>{
    public:
        session(tcp::socket socket) : socket_(std::move(socket)){}

        void start();

        void do_read();

        void do_write(string origin_Msg);

        tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
        int clientNo = 0;
        int exitClient = 0;
};

void panelCgi(shared_ptr<session> Session);

int consoleCgi(shared_ptr<session> Session);

//http_server
typedef struct environmentVars{
    const string names[9] = {
        "REQUEST_METHOD",
        "REQUEST_URI",
        "QUERY_STRING",
        "SERVER_PROTOCOL",
        "HTTP_HOST",
        "SERVER_ADDR",
        "SERVER_PORT",
        "REMOTE_ADDR",
        "REMOTE_PORT"
    };
    string values[9];
}EnvVars;

EnvVars envVars;
char **test_argv;

void parseHttpRequest(string HttpRequest);

void setHttpEnv();

//console.cgi
typedef struct requestData{
    string url;
    string port;
    string testfile;
}requestData;

typedef struct clientData{
    boost::asio::io_context io_context;
    // auto endpoints;
    string ID;
    string testfile;
}clientData;

requestData requestDatas[5];

void parse_QUERY_STRING(string &QUERY_STRING);

void send_default_HTML(shared_ptr<session> Session);

void send_dafault_table(shared_ptr<session> Session, string index, string Msg);

void send_command(shared_ptr<session> Session, string index, string Msg);

void send_shell(shared_ptr<session> Session, string index, string Msg);

void refactor(string &Msg);