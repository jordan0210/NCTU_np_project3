#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include <string.h>
#include <sys/wait.h>

using boost::asio::ip::tcp;
using namespace std;

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