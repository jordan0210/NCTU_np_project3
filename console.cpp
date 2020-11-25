#include "console.h"

class client{
    public:
        client(boost::asio::io_context& io_context,
            const tcp::resolver::results_type& endpoints,
            string ID)
            : io_context_(io_context), socket_(io_context), ID_(ID){

            do_connect(endpoints);
        }

        // void close(){
        //     boost::asio::post(io_context_, [this]() { socket_.close(); });
        // }

    private:
        void do_connect(const tcp::resolver::results_type& endpoints){
            boost::asio::async_connect(socket_, endpoints,
                [this](boost::system::error_code ec, tcp::endpoint){

                if (!ec){
                    bzero(data_, sizeof(data_));
                    string path = "./test_case/" + requestDatas[stoi(ID_)].testfile;
                    int fd_testfile = open(path.data(), O_RDONLY);
                    dup2(fd_testfile, STDIN_FILENO);
                    close(fd_testfile);
                    do_read();
                }
            });
        }

        void do_read(){
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                [this](boost::system::error_code ec, std::size_t length){

                if (!ec){
                    data_[length] = '\0';
                    string Msg = data_;
                    bzero(data_, sizeof(data_));

                    send_shell(ID_, Msg);
                    if (length != 0){
                        if ((int)Msg.find('%', 0) < 0){
                            do_read();
                        } else {
                            string command;
                            getline(cin, command);
                            command += "\n";
                            send_command(ID_, command);
                            do_write(command);
                        }
                    }
                } else {
                    socket_.close();
                }
            });

        }

        void do_write(string origin_Msg){
            const char *Msg = origin_Msg.c_str();
            char unit;
            boost::asio::async_write(socket_, boost::asio::buffer(Msg, sizeof(unit)*origin_Msg.length()),
                [this, &origin_Msg](boost::system::error_code ec, std::size_t /*length*/){

                if (!ec){
                    if ((int)origin_Msg.find("exit", 0) >){
                        do_read();
                    } else {
                        string test = "-------Find exit.-------";
                        send_command(ID_, test);
                    }
                }
            });
        }

        boost::asio::io_context& io_context_;
        tcp::socket socket_;
        string ID_;
        enum { max_length = 1024 };
        char data_[max_length];
};

int main(){
    string QUERY_STRING = getenv("QUERY_STRING");
    parse_QUERY_STRING(QUERY_STRING);
    send_default_HTML();

    try{
        for (int i=0; i<5; i++){
            if (requestDatas[i].url.length() == 0)
                continue;
            send_dafault_table(to_string(i), (requestDatas[i].url + ":" + requestDatas[i].port));

            boost::asio::io_context io_context;
            tcp::resolver resolver(io_context);
            auto endpoints = resolver.resolve(requestDatas[i].url.data(), requestDatas[i].port.data());
            client c(io_context, endpoints, to_string(i));

            int child_pid;
            while((child_pid = fork()) < 0){
                while(waitpid(-1, NULL, WNOHANG) > 0){}
            }

            switch (child_pid){
                case 0:
                    io_context.run();
                    return 0;
                default:
                    waitpid(-1, NULL, WNOHANG);
            }
        }
    } catch (exception& e){
        cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}

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

void send_default_HTML(){
    cout << "Content-type: text/html\r\n\r\n";
    cout << "\
    <!DOCTYPE html>\
    <html lang=\"en\">\
        <head>\
            <meta charset=\"UTF-8\" />\
            <title>NP Project 3 Sample Console</title>\
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
    cout.flush();
}

void send_dafault_table(string index, string Msg){
    Msg = "<th scope=\\\"col\\\">" + Msg + "</th>";
    cout << "<script>document.getElementById('tableHead').innerHTML += '" << Msg << "';</script>";
    cout.flush();
    Msg = "<td><pre id=\\\"s" + index + "\\\" class=\\\"mb-0\\\"></pre></td>";
    cout << "<script>document.getElementById('tableBody').innerHTML += '" << Msg << "';</script>";
    cout.flush();
}

void send_command(string index, string Msg){
    // cmdCount++;
    // Msg = to_string(cmdCount) + " : " + Msg;
    refactor(Msg);
    cout << "<script>document.getElementById('s" + index + "').innerHTML += '<b>" << Msg << "</b>';</script>";
    cout.flush();
}

void send_shell(string index, string Msg){
    refactor(Msg);
    cout << "<script>document.getElementById('s" + index + "').innerHTML += '" << Msg << "';</script>";
    cout.flush();
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
        }

        else {
            returnMsg += Msg[i];
        }
    }
    Msg = move(returnMsg);
}