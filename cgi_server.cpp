#include "cgi_server.h"

void session::start(){
    do_read();
}

void session::do_read(){
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length){

<<<<<<< HEAD
        if (!ec){
            string HttpRequest = data_;

            parseHttpRequest(HttpRequest);
            string temp_URI = envVars.values[1] + "?";
            string URI = temp_URI.substr(0, temp_URI.find('?', 0));

            if (URI == "/panel.cgi"){
                panelCgi(self);
            } else if (URI == "/console.cgi"){
                consoleCgi(self);
            }
            socket_.close();
=======
    // private:
        void do_read(){
            auto self(shared_from_this());
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                [this, self](boost::system::error_code ec, std::size_t length){

                if (!ec){
                    string HttpRequest = data_;

                    parseHttpRequest(HttpRequest);
                    string temp_URI = envVars.values[1] + "?";
                    string URI = temp_URI.substr(0, temp_URI.find('?', 0));

                    if (URI == "/panel.cgi"){
                        panelCgi();
                        acceptable = true;
                    } else if (URI == "/console.cgi"){
                        consoleCgi();
                        acceptable = true;
                    }
                }
            });
>>>>>>> 8b36c68c14bee91c0228d3157523c6cb66401a3d
        }
    });
}

<<<<<<< HEAD
void session::do_write(string origin_Msg){
    auto self(shared_from_this());
    const char *Msg = origin_Msg.c_str();
    boost::asio::async_write(socket_, boost::asio::buffer(Msg, sizeof('\n')*origin_Msg.length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/){

        if (!ec){
            // do_read();
=======
        void do_write(string origin_Msg){
            auto self(shared_from_this());
            const char *Msg = origin_Msg.c_str();
            boost::asio::async_write(socket_, boost::asio::buffer(Msg, sizeof('\n')*origin_Msg.length()),
                [this, self](boost::system::error_code ec, std::size_t /*length*/){

                if (!ec){
                    // do_read();
                }
            });
>>>>>>> 8b36c68c14bee91c0228d3157523c6cb66401a3d
        }
    });
}

std::shared_ptr<session> globalSession;

class server{
    public:
        server(boost::asio::io_context& io_context, short port)
            : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)){
            do_accept();
        }

    private:
        void do_accept(){
            acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket){
                if (!ec && acceptable){
                    acceptable = false;
                    globalSession = std::make_shared<session>(std::move(socket));
                    globalSession->start();
                }

                do_accept();
            });
        }

        tcp::acceptor acceptor_;
};

class client
    : public std::enable_shared_from_this<client>{
    private:
        tcp::resolver resolver;
        tcp::socket socket;
        boost::asio::io_context& io_context;
        shared_ptr<session> Session;
        boost::asio::ip::tcp::resolver::results_type endpoints;
        string ID;
        ifstream fin;
        enum { max_length = 4096 };
        char data_[max_length];

    public:
        client(boost::asio::io_context& io_context,
            string ID,
            shared_ptr<session> Session)
            : resolver(io_context), socket(io_context), io_context(io_context), Session(Session), ID(ID){}

        void start(){
            do_resolve();
        }

    private:
        void do_resolve(){
            auto self(shared_from_this());
            resolver.async_resolve(requestDatas[stoi(ID)].url, requestDatas[stoi(ID)].port,
                [this, self](const boost::system::error_code &ec,
                    const boost::asio::ip::tcp::resolver::results_type results){
                    if (!ec){
                        endpoints = results;
                        do_connect();
                    } else {
                        socket.close();
                    }
                }
            );
        }

        void do_connect(){
            auto self(shared_from_this());
            boost::asio::async_connect(socket, endpoints,
                [this, self](const boost::system::error_code &ec, tcp::endpoint ed){

                if (!ec){
                    memset(data_, '\0', sizeof(data_));
                    string path = "./test_case/" + requestDatas[stoi(ID)].testfile;
                    cerr << path << endl;
                    fin.open(path.data());
                    do_read();
                } else {
                    socket.close();
                }
            });
        }

        void do_read(){
            auto self(shared_from_this());
            socket.async_read_some(boost::asio::buffer(data_, max_length),
                [this, self](boost::system::error_code ec, std::size_t length){

                if (!ec){
                    data_[length] = '\0';
                    string Msg = data_;
                    memset(data_, '\0', sizeof(data_));

                    send_shell(Session, ID, Msg);
                    if (length != 0){
                        if ((int)Msg.find('%', 0) < 0){
                            do_read();
                        } else {
                            string command;
                            getline(fin, command);
                            command += "\n";
                            send_command(Session, ID, command);
                            do_write(command);
                        }
                    }
                } else {
                    socket.close();
                }
            });
        }

        void do_write(string origin_Msg){
            auto self(shared_from_this());
            const char *Msg = origin_Msg.c_str();
            boost::asio::async_write(socket, boost::asio::buffer(Msg, sizeof('\n')*origin_Msg.length()),
                [this, self, origin_Msg](boost::system::error_code ec, std::size_t /*length*/){

                if (!ec){
                    if (origin_Msg.compare("exit\n")){
                        do_read();
                    } else {
                        socket.close();
                        (this->Session)->exitClient++;
                        if ((this->Session)->exitClient == (this->Session)->clientNo){
                            ((this->Session)->socket_).close();
                        }
                    }
                } else {
                    socket.close();
                }
            });
        }
};

int main(int argc, char* argv[]){
    test_argv = argv;
    try{
        if (argc != 2){
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        // boost::asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        io_context.run();
    } catch (std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }

  return 0;
}

<<<<<<< HEAD
void panelCgi(shared_ptr<session> Session){
=======
void panelCgi(){
>>>>>>> 8b36c68c14bee91c0228d3157523c6cb66401a3d
    string Msg =  "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";// << flush;

    string host_menu = "";
    for (int i=1; i<13; i++){
        host_menu = host_menu + "<option value=\"nplinux" + to_string(i) + ".cs.nctu.edu.tw\">nplinux" + to_string(i) + "</option>";
    }

    string test_case_menu = "";
    for (int i=1; i<11; i++){
        test_case_menu = test_case_menu + "<option value=\"t" + to_string(i) + ".txt\">t" + to_string(i) + ".txt</option>";
    }

    Msg += "\
    <!DOCTYPE html>\
    <html lang=\"en\">\
        <head>\
            <title>NP Project 3 Panel</title>\
            <link\
                rel=\"stylesheet\"\
                href=\"https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/css/bootstrap.min.css\"\
                integrity=\"sha384-TX8t27EcRE3e/ihU7zmQxVncDAy5uIKz4rEkgIXeMed4M0jlfIDPvg6uqKI2xXr2\"\
                crossorigin=\"anonymous\"\
            />\
            <link\
                href=\"https://fonts.googleapis.com/css?family=Source+Code+Pro\"\
                rel=\"stylesheet\"\
            />\
            <link\
                rel=\"icon\"\
                type=\"image/png\"\
                href=\"https://cdn4.iconfinder.com/data/icons/iconsimple-setting-time/512/dashboard-512.png\"\
            />\
            <style>\
                * {\
                    font-family: 'Source Code Pro', monospace;\
                }\
            </style>\
        </head>\
        <body class=\"bg-secondary pt-5\">\
            <form action=\"console.cgi\" method=\"GET\">\
                <table class=\"table mx-auto bg-light\" style=\"width: inherit\">\
                    <thead class=\"thead-dark\">\
                        <tr>\
                            <th scope=\"col\">#</th>\
                            <th scope=\"col\">Host</th>\
                            <th scope=\"col\">Port</th>\
                            <th scope=\"col\">Input File</th>\
                        </tr>\
                    </thead>\
                    <tbody>";// << flush;

    for (int i=0; i<5; i++){
        Msg = Msg + "\
                        <tr>\
                            <th scope=\"row\" class=\"align-middle\">Session " + to_string(i + 1) + "</th>\
                            <td>\
                                <div class=\"input-group\">\
                                    <select name=\"h" + to_string(i) + "\" class=\"custom-select\">\
                                        <option></option>" + host_menu + "\
                                    </select>\
                                    <div class=\"input-group-append\">\
                                        <span class=\"input-group-text\">.cs.nctu.edu.tw</span>\
                                    </div>\
                                </div>\
                            </td>\
                            <td>\
                                <input name=\"p" + to_string(i) + "\" type=\"text\" class=\"form-control\" size=\"5\" />\
                            </td>\
                            <td>\
                                <select name=\"f" + to_string(i) + "\" class=\"custom-select\">\
                                    <option></option>" + test_case_menu + "\
                                </select>\
                            </td>\
                        </tr>";// << flush;
    }

    Msg += "\
                        <tr>\
                            <td colspan=\"3\"></td>\
                            <td>\
                                <button type=\"submit\" class=\"btn btn-info btn-block\">Run</button>\
                            </td>\
                        </tr>\
                    </tbody>\
                </table>\
            </form>\
        </body>\
    </html>";// << flush;
<<<<<<< HEAD
    Session->do_write(Msg);
}

int consoleCgi(shared_ptr<session> Session){
    parse_QUERY_STRING(envVars.values[2]);
    send_default_HTML(Session);
=======
    globalSession->do_write(Msg);
    (globalSession->socket_).close();
}

int consoleCgi(){
    cerr << envVars.values[2] << endl;
    parse_QUERY_STRING(envVars.values[2]);
    send_default_HTML();
>>>>>>> 8b36c68c14bee91c0228d3157523c6cb66401a3d

    try{
        // boost::asio::io_context io_context;
        Session->clientNo = 5;
        for (int i=0; i<5; i++){
            if (requestDatas[i].url == ""){
                Session->clientNo = i;
                break;
            }
            send_dafault_table(Session, to_string(i), (requestDatas[i].url + ":" + requestDatas[i].port));
            make_shared<client>(io_context, to_string(i), Session)->start();
        }
        io_context.run();
        cerr << "consoleCgi done.\n";
        (Session->socket_).close();
    } catch (exception& e){
        cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}

// -------http_server-------
void parseHttpRequest(string HttpRequest){
    int firstLineIndex = HttpRequest.find('\n', 0);
    int secondLineIndex = HttpRequest.find('\n', firstLineIndex + 1);
    string Line1 = HttpRequest.substr(0, firstLineIndex-1);
    string Line2 = HttpRequest.substr(firstLineIndex+1, secondLineIndex - firstLineIndex - 2);

    int startIndex, endIndex;
    startIndex = 0;
    endIndex = Line1.find(' ', 0);
    envVars.values[0] = Line1.substr(startIndex, endIndex - startIndex);

    startIndex = endIndex + 1;
    if ((endIndex = Line1.find('?', startIndex)) != -1){
        endIndex = Line1.find('?', startIndex);
        envVars.values[1] = Line1.substr(startIndex, endIndex - startIndex);

        startIndex = endIndex + 1;
        endIndex = Line1.find(' ', startIndex);
        envVars.values[2] = Line1.substr(startIndex, endIndex - startIndex);
        envVars.values[1] += "?" + envVars.values[2];
    } else {
        endIndex = Line1.find(' ', startIndex);
        envVars.values[1] = Line1.substr(startIndex, endIndex - startIndex);

        envVars.values[2] = "";
    }

    startIndex = endIndex + 1;
    envVars.values[3] = Line1.substr(startIndex);

    Line2 = Line2.substr(Line2.find(' ', 0) + 1);
    envVars.values[4] = Line2;
    envVars.values[5] = Line2.substr(0, Line2.find(':', 0));
    envVars.values[6] = Line2.substr(Line2.find(':', 0) + 1);
}

// -------------------------

// -------console.cgi-------
void parse_QUERY_STRING(string &QUERY_STRING){
    QUERY_STRING = QUERY_STRING + "&";
    string requestBlock;
    int start, end, index;
    start = 0;
    index = 0;
    while ((end = QUERY_STRING.find('&', start)) != -1){
        requestBlock = QUERY_STRING.substr(start, end-start);
        if (requestBlock.length() == 3)
            break;
        if (index % 3 == 0){
            requestDatas[index/3].url = requestBlock.substr(3);
        } else if (index % 3 == 1){
            requestDatas[index/3].port = requestBlock.substr(3);
        } else {
            requestDatas[index/3].testfile = requestBlock.substr(3);
        }
        index++;
        start = end + 1;
    }
}

<<<<<<< HEAD
void send_default_HTML(shared_ptr<session> Session){
=======
void send_default_HTML(){
>>>>>>> 8b36c68c14bee91c0228d3157523c6cb66401a3d
    string Msg = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
    Msg += "\
    <!DOCTYPE html>\
    <html lang=\"en\">\
        <head>\
            <meta charset=\"UTF-8\" />\
            <title>NP Project 3 Console</title>\
            <link\
                rel=\"stylesheet\"\
                href=\"https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/css/bootstrap.min.css\"\
                integrity=\"sha384-TX8t27EcRE3e/ihU7zmQxVncDAy5uIKz4rEkgIXeMed4M0jlfIDPvg6uqKI2xXr2\"\
                crossorigin=\"anonymous\"\
            />\
            <link\
                href=\"https://fonts.googleapis.com/css?family=Source+Code+Pro\"\
                rel=\"stylesheet\"\
            />\
            <link\
                rel=\"icon\"\
                type=\"image/png\"\
                href=\"https://cdn0.iconfinder.com/data/icons/small-n-flat/24/678068-terminal-512.png\"\
            />\
            <style>\
            * {\
                font-family: 'Source Code Pro', monospace;\
                font-size: 1rem !important;\
            }\
            body {\
                background-color: #212529;\
            }\
            pre {\
                color: #cccccc;\
            }\
            b {\
                color: #01b468;\
            }\
            </style>\
        </head>\
        <body>\
            <table class=\"table table-dark table-bordered\">\
                <thead>\
                    <tr id=\"tableHead\">\
                    </tr>\
                </thead>\
                <tbody>\
                    <tr id=\"tableBody\">\
                    </tr>\
                </tbody>\
            </table>\
        </body>\
    </html>";
<<<<<<< HEAD
    Session->do_write(Msg);
=======
    globalSession->do_write(Msg);
>>>>>>> 8b36c68c14bee91c0228d3157523c6cb66401a3d
    // cout.flush();
}

void send_dafault_table(shared_ptr<session> Session, string index, string Msg){
    Msg = "<th scope=\\\"col\\\">" + Msg + "</th>";
    Msg = "<script>document.getElementById('tableHead').innerHTML += '" + Msg + "';</script>";
<<<<<<< HEAD
    Session->do_write(Msg);
    // cout.flush();
    Msg = "<td><pre id=\\\"s" + index + "\\\" class=\\\"mb-0\\\"></pre></td>";
    Msg = "<script>document.getElementById('tableBody').innerHTML += '" + Msg + "';</script>";
    Session->do_write(Msg);
    // cout.flush();
}

void send_command(shared_ptr<session> Session, string index, string Msg){
    refactor(Msg);
    Msg = "<script>document.getElementById('s" + index + "').innerHTML += '<b>" + Msg + "</b>';</script>";
    Session->do_write(Msg);
=======
    globalSession->do_write(Msg);
    // cout.flush();
    Msg = "<td><pre id=\\\"s" + index + "\\\" class=\\\"mb-0\\\"></pre></td>";
    Msg = "<script>document.getElementById('tableBody').innerHTML += '" + Msg + "';</script>";
    globalSession->do_write(Msg);
    // cout.flush();
}

void send_command(string index, string Msg){
    refactor(Msg);
    Msg = "<script>document.getElementById('s" + index + "').innerHTML += '<b>" + Msg + "</b>';</script>";
    globalSession->do_write(Msg);
>>>>>>> 8b36c68c14bee91c0228d3157523c6cb66401a3d
    // cout.flush();
}

void send_shell(shared_ptr<session> Session, string index, string Msg){
    refactor(Msg);
    Msg = "<script>document.getElementById('s" + index + "').innerHTML += '" + Msg + "';</script>";
<<<<<<< HEAD
    Session->do_write(Msg);
=======
    globalSession->do_write(Msg);
>>>>>>> 8b36c68c14bee91c0228d3157523c6cb66401a3d
    // cout.flush();
}

void refactor(string &Msg){
    string returnMsg = "";
    for (int i=0; i<(int)Msg.length(); i++){
        if (Msg[i] == '\n'){
            returnMsg += "<br>";
        } else if (Msg[i] == '\r'){
            returnMsg += "";
        } else if (Msg[i] == '\''){
            returnMsg += "\\'";
        } else if (Msg[i] == '<'){
            returnMsg += "&lt;";
        } else if (Msg[i] == '>'){
            returnMsg += "&gt;";
        } else if (Msg[i] == '&'){
            returnMsg += "&amp;";
        }

        else {
            returnMsg += Msg[i];
        }
    }
    Msg = move(returnMsg);
}
// -------------------------