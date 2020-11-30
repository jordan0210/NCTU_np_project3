#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include <fstream>
#include <string.h>

using boost::asio::ip::tcp;
using namespace std;

void panelCgi();

int consoleCgi();

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
bool acceptable = true;

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

void send_default_HTML();

void send_dafault_table(string index, string Msg);

void send_command(string index, string Msg);

void send_shell(string index, string Msg);

void refactor(string &Msg);