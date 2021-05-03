#include <iostream>
#include <dirent.h>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/wait.h> 
#include <iterator>
#include <sstream>
#include <sys/time.h>
#include <sys/msg.h>

#define MAX_ROWS 256

using namespace std;

string read_message_from_pipe(int pipe) { 
    char msg[1000];
    read(pipe, msg, 1000);
    string message(msg);
    return message;
}
void write_on_pipe(string pipe, string message) {
    int fd = open(pipe.c_str(), O_TRUNC | O_WRONLY );
    write(fd, message.c_str(), message.size()+ 1);
    close(fd);
}
vector<string> tokenize(string message) {
    stringstream s_stream(message);
    istream_iterator <string> begin(s_stream);
    istream_iterator <string> end;
    vector <string> command_tokens(begin, end);
    return command_tokens;    
}
int search_LUT(vector < vector<int> > LUT, string system) {
    for(int i = 0; i < LUT.size(); i++) {
        if(LUT[i][0] == stoi(system)) return LUT[i][1];
    }
    return -1;
}

void broadcast(string message, string switch_num, int ports_num) {
    for(int i = 0; i < ports_num; i++) {
        string pipe = "./switch_" + switch_num + "_port_" + to_string(i) + ".pipe";
        write_on_pipe(pipe, message);
    }
}

int main(int argc, char **argv) {
    vector<string> reading_list;
    vector<char> writing_list;
    string switch_num(argv[1]);
    string ports_num(argv[2]);
    vector < vector<int> > LUT;

    for (int i = 0; i < MAX_ROWS; i++) {
        vector<int> vec_row;
        vec_row.push_back(-1);
        vec_row.push_back(-1);
        LUT.push_back(vec_row);
    }

    int switch_number = stoi(switch_num);
    int number_of_ports = stoi(ports_num);
    cout << "New switch process created -- Num: " << switch_number << " number of Ports: " << number_of_ports << endl;
    string manager_pipe = "./manager_switch_" + switch_num + ".pipe";
    reading_list.push_back(manager_pipe);

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int last_open_file = 0;
    int last_fd;
    fd_set rfds;
    FD_ZERO(&rfds);
    while (1) {
        vector<string> new_files;
        vector<vector<int> > open_files;
        cout << "last" << last_open_file <<endl;
        cout << "size" << reading_list.size() << endl;
        for (int i = last_open_file; i < reading_list.size(); i++) {
            int pipe = open(reading_list[i].c_str(), O_RDONLY | O_NONBLOCK);    
            vector<int> row;
            row.push_back(pipe);
            row.push_back(i);
            open_files.push_back(row);
            FD_SET(pipe, &rfds);
            last_open_file++;
            last_fd = pipe;
            cout << last_fd << endl;
        }

        int fd = select(last_fd + 1, &rfds, NULL, NULL, &tv);
        cout << "fd " << fd << endl;
        if(!fd) continue;

        string message = read_message_from_pipe(fd);

        int ind;
        for(int j = 0; j < open_files.size(); j++) {
            if(open_files[j][0] == fd) {
                ind = open_files[j][1];
                break;
            }
        }

        if (reading_list[ind] == manager_pipe) {
            vector <string> command_tokens = tokenize(message);
            if (command_tokens[0] == "CONNECTED_TO_SWITCH" || command_tokens[0] == "CONNECTED_TO_SYSTEM") {
                string reading_pipe_name = command_tokens[1];
                mkfifo(reading_pipe_name.c_str(), 0666);
                new_files.push_back(reading_pipe_name);
                cout << "Pipe created with name: " << reading_pipe_name << endl;
            }
        }
        else {
            vector <string> frame = tokenize(message);
            string source = frame[0];
            int port = search_LUT(LUT, source);
            if(port == -1) {
                broadcast(message, switch_num, stoi(ports_num));
            }
            else {
                string sending_pipe = "./switch_" + to_string(switch_number) + "_port_" + to_string(port) + ".pipe";
                write_on_pipe(sending_pipe, message);
            }
        }

        for (int j = 0; j < new_files.size(); j++) reading_list.push_back(new_files[j]);
    }
    return 0;
}