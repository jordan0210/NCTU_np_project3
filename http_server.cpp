#include "http_server.h"

class session
    : public std::enable_shared_from_this<session>{
    public:
        session(tcp::socket socket)
            : socket_(std::move(socket)){
        }

        void start(){
            do_read();
        }

    private:
        void do_read(){
            auto self(shared_from_this());
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                [this, self](boost::system::error_code ec, std::size_t length){

                if (!ec){
                    data_[length] = '\0';
                    string HttpRequest = data_;
                    bzero(data_, sizeof(data_));

                    parseHttpRequest(HttpRequest);

                    int child_pid;
                    while((child_pid = fork()) < 0){
                        while(waitpid(-1, NULL, WNOHANG) > 0){}
                    }

                    string temp_URI, path;
                    switch (child_pid){
                        case 0:
                            setHttpEnv();
                            setenv("SERVER_ADDR", socket_.local_endpoint().address().to_string().c_str(), 1);
                            setenv("REMOTE_ADDR", socket_.remote_endpoint().address().to_string().c_str(), 1);
                            setenv("REMOTE_PORT", to_string(htons(socket_.remote_endpoint().port())).c_str(), 1);
                            dup2(socket_.native_handle(), STDOUT_FILENO);
                            cout << "HTTP/1.1 200 OK\r\n" << flush;

                            socket_.close();
                            temp_URI = envVars.values[1] + "?";
                            path = "." + temp_URI.substr(0, temp_URI.find('?', 0));
                            if (execv(path.data(), test_argv) == -1){

                            }
                            exit(0);
                        default:
                            socket_.close();
                            waitpid(child_pid, NULL, WNOHANG);
                    }
                }
            });
        }

        void do_write(std::size_t length){
            auto self(shared_from_this());
            boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                [this, self](boost::system::error_code ec, std::size_t /*length*/){

                if (!ec){
                    do_read();
                }
            });
        }

        tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
};

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
                if (!ec){
                    std::make_shared<session>(std::move(socket))->start();
                }

                do_accept();
            });
        }

        tcp::acceptor acceptor_;
};

int main(int argc, char* argv[]){
    test_argv = argv;
    try{
        if (argc != 2){
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        io_context.run();
    } catch (std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }

  return 0;
}

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
        envVars.values[1] = envVars.values[1] + "?" + envVars.values[2];
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

void setHttpEnv(){
    for (int i=0; i<7; i++){
        setenv(envVars.names[i].data(), envVars.values[i].data(), 1);
    }
}