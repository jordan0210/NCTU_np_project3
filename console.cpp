#include <iostream>
#include <string.h>

using namespace std;

typedef struct requestData{
    string url;
    string port;
    string testfile;
}requestData;

requestData requestDatas[5];

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
    cout << "<script>document.getElementById('s" + index + "').innerHTML += '" << Msg << "';</script>";
    cout.flush();
}

void send_shell(string index, string Msg){
    cout << "<script>document.getElementById('s" + index + "').innerHTML += '<b>" << Msg << "</b>';</script>";
    cout.flush();
}

int main(){
    setenv("QUERY_STRING", "h0=nplinux1.cs.nctu.edu.tw&p0=1234&f0=t1.txt&h1=nplinux2.cs.nctu.edu.tw&p1=5678&f1=t2.txt&h2=nplinux3.cs.nctu.edu.tw&p2=1234&f2=t3.txt&h3=nplinux4.cs.nctu.edu.tw&p3=1234&f3=t4.txt&h4=nplinux5.cs.nctu.edu.tw&p4=1234&f4=t5.txt", 1);
    string QUERY_STRING = getenv("QUERY_STRING");
    parse_QUERY_STRING(QUERY_STRING);
    send_default_HTML();
    for (int i=0; i<5; i++){
        if (requestDatas[i].url.length() == 0)
            break;
        send_dafault_table(to_string(i), (requestDatas[i].url + ":" + requestDatas[i].port));
        int child_pid;
        while((child_pid = fork()) < 0){
			while(waitpid(-1, NULL, WNOHANG) > 0){}
		}
		switch (child_pid){
			case 0 :
                int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
                struct sockaddr_in serverAddr;
                socklen_t addr_size;

                serverAddr.sin_family = AF_INET;
                serverAddr.sin_port = htons(stoi(requestDatas[i].port));
                serverAddr.sin_addr.s_addr = inet_addr(requestDatas[i].url);
                memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    }
}