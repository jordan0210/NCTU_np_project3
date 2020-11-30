#include "console.h"

class client : public std::enable_shared_from_this<client>{
    private:
        tcp::resolver resolver;
        tcp::socket socket;
        boost::asio::io_context& io_context;
        boost::asio::ip::tcp::resolver::results_type endpoints;
        string ID;
        ifstream fin;
        enum { max_length = 4096 };
        char data_[max_length];

    public:
        client(boost::asio::io_context& io_context,
            string ID)
            : resolver(io_context), socket(io_context), io_context(io_context), ID(ID){}

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
                    bzero(data_, sizeof(data_));
                    string path = "./test_case/" + requestDatas[stoi(ID)].testfile;
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
                    bzero(data_, sizeof(data_));

                    send_shell(ID, Msg);
                    if (length != 0){
                        if ((int)Msg.find('%', 0) < 0){
                            do_read();
                        } else {
                            string command;
                            getline(fin, command);
                            command += "\n";
                            send_command(ID, command);
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
                    }
                }
            });
        }
};

int main(){
    string QUERY_STRING = getenv("QUERY_STRING");
    parse_QUERY_STRING(QUERY_STRING);
    send_default_HTML();

    try{
        boost::asio::io_context io_context;
        for (int i=0; i<5; i++){
            if (requestDatas[i].url == "")
                break;
            send_dafault_table(to_string(i), (requestDatas[i].url + ":" + requestDatas[i].port));
            make_shared<client>(io_context, to_string(i))->start();
        }
        io_context.run();
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