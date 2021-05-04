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

using namespace std;

string read_message_from_pipe(int pipe) { 
    char msg[1000];
    read(pipe, msg, 1000);
    string message(msg);
    return message;
}
vector<string> tokenize(string message) {
    stringstream s_stream(message);
    istream_iterator <string> begin(s_stream);
    istream_iterator <string> end;
    vector <string> command_tokens(begin, end);
    return command_tokens;    
}
vector<string> make_frames(string file, string source, string destination) {
    int pipe = open(file.c_str(), O_RDONLY | O_NONBLOCK);
    char temp[100000];
    read(pipe, temp, 100000);
    string msg(temp);
    vector<string> frames;
    while(1) {
        string frame;
        frame = source + " " + destination + " ";
        if(msg.size() <= 50) { 
            frame+=msg;
            frames.push_back(frame);
            return frames;
        }
        for(string::size_type i = 0; i < 50; ++i) {
            frame += msg[i];
        }
        frames.push_back(frame);
        msg.erase(0,50);
    }
}
void write_on_pipe(string pipe, string message) {
    int fd = open(pipe.c_str(), O_TRUNC | O_WRONLY );
    cout << "open " << pipe << endl;
    cout << "msg is: " << message << endl;
    write(fd, message.c_str(), message.size()+ 1);
    cout << "message wrote" << endl;
    close(fd);
    cout << "file closed" << endl;
    return;
}
int main(int argc, char **argv) {
    vector<string> pipe_file_names;
    string system_num(argv[1]);
    string manager_pipe = "./manager_system_" + system_num + ".pipe";

    pipe_file_names.push_back(manager_pipe);
    cout << "New system process created -- Num: " << system_num << endl;

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    string switch_reading_pipe;
    string switch_writing_pipe;

    fd_set rfds;
    int max_fd;
    while (1) {
        FD_ZERO(&rfds);
        int manager = open(manager_pipe.c_str(), O_RDONLY | O_NONBLOCK);
        FD_SET(manager, &rfds);
        max_fd = manager;
        if(switch_reading_pipe != "") {
            int switch_p = open(switch_reading_pipe.c_str(), O_RDONLY | O_NONBLOCK);
            FD_SET(switch_p, &rfds);
            max_fd = switch_p;
        }
        if (!select(max_fd + 1, &rfds, NULL, NULL, &tv)) {
            if(switch_reading_pipe != "") {
                close(max_fd);
            }
            close(manager);
            continue;
        }
        if(FD_ISSET(manager, &rfds)) {
            string message = read_message_from_pipe(manager);
            vector <string> command_tokens = tokenize(message);
            if (command_tokens[0] == "CONNECTED_TO_SWITCH") {
                switch_reading_pipe = command_tokens[1];
                mkfifo(switch_reading_pipe.c_str(), 0666);
                cout << "Pipe created with name: " << switch_reading_pipe << endl;
                switch_writing_pipe = "./system_" + system_num + "_" + switch_reading_pipe.erase(0,2) ;
            }
            else if(command_tokens[0] == "SEND") {
                string destination = command_tokens[2];
                string file = command_tokens[1];
                vector<string> frames = make_frames(file, system_num, destination);
                for(int i = 0; i < frames.size(); i++) {
                    write_on_pipe(switch_writing_pipe,frames[i]);
                    cout << "frame " << i << " wrote on " << switch_writing_pipe << endl;
                    sleep(2);
                }
            }
        }
        close(manager);
    }
    return 0;
}