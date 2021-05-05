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
string search_writings(vector < vector<string> > writing_list, string switch_num) {
    for(int i = 0; i < writing_list.size(); i++) {
        if(writing_list[i][0] == switch_num) return writing_list[i][1];
    }
    return "";
}
void broadcast(string message, string switch_num, int ports_num, int source_port) {
    for(int i = 0; i < ports_num; i++) {
        if(source_port == i+1) continue;
        string pipe = "./switch_" + switch_num + "_port_" + to_string(i+1) + ".pipe";
        write_on_pipe(pipe, message);
    }
}
int main(int argc, char **argv) {
    vector<string> reading_list;
    vector<vector<string> > writing_list;
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

    fd_set rfds;
    int lut_ind = 0;
    int f_test = 0;
    while (1) {
        vector<string> new_files;
        vector<int> open_files;
        string message;

        FD_ZERO(&rfds);
        for (int i = 0; i < reading_list.size(); i++) {
            int pipe = open(reading_list[i].c_str(), O_RDONLY | O_NONBLOCK);
            FD_SET(pipe, &rfds);
            open_files.push_back(pipe);
        }

        if (!select(open_files.back() + 1, &rfds, NULL, NULL, &tv)) {
            for (int j = 0; j < open_files.size(); j++) close(open_files[j]);
            continue;
        }
        for (int i = 0; i < open_files.size(); i++) {
            int fd = open_files[i];
            if(FD_ISSET(fd, &rfds)){
                if(reading_list[i] == manager_pipe) { 
                    message = read_message_from_pipe(fd);
                    vector <string> command_tokens = tokenize(message);
                    if (command_tokens[0] == "CONNECTED_TO_SYSTEM") {
                        string reading_pipe_name = command_tokens[1];
                        mkfifo(reading_pipe_name.c_str(), 0666);
                        new_files.push_back(reading_pipe_name);
                        cout << "Pipe created with name: " << reading_pipe_name << endl;
                    }
                    if (command_tokens[0] == "CONNECTED_TO_SWITCH") {
                        string reading_pipe_name = command_tokens[1];
                        mkfifo(reading_pipe_name.c_str(), 0666);
                        new_files.push_back(reading_pipe_name);
                        vector <string> tokens;
                        stringstream check(reading_pipe_name);
                        string ss;
                        while(getline(check, ss, '_'))
                        {
                            tokens.push_back(ss);
                        }
                        vector<string> row;
                        row.push_back(tokens[1]);
                        row.push_back(command_tokens[3]);
                        writing_list.push_back(row);
                        cout << "Pipe created with name: " << reading_pipe_name << endl;
                    }
                    if (command_tokens[0] == "DISCONNECT") {
                        
                    }
                }
                else {
                    f_test++;
                    message = read_message_from_pipe(fd);
                    vector <string> frame = tokenize(message);
                    string destination = frame[1];
                    string source = frame[0];
                    int port = search_LUT(LUT, destination);
                    int source_port;
                    if(search_LUT(LUT,source) == -1) {
                        vector <string> tokens;
                        stringstream check(reading_list[i]);
                        string ss;
                        while(getline(check, ss, '_'))
                        {
                            tokens.push_back(ss);
                        }

                        LUT[lut_ind][0] = stoi(source);
                        if(tokens[0] == "./system") {
                            LUT[lut_ind][1] = stoi(tokens[5]);
                            source_port = stoi(tokens[5]);
                        }
                        else{
                            string w_port = search_writings(writing_list, tokens[1]);
                            LUT[lut_ind][1] = stoi(w_port);
                            source_port = LUT[lut_ind][1];
                        }
                        cout << "learning, system: " << LUT[lut_ind][0] << " from port: " << source_port << endl;
                        lut_ind++;
                    }
                    if(port == -1) {
                        broadcast(message, switch_num, stoi(ports_num), source_port);
                        cout << "switch " << switch_num << " broadcasted the frame!" << endl;
                    }
                    else {
                        string sending_pipe = "./switch_" + to_string(switch_number) + "_port_" + to_string(port) + ".pipe";
                        write_on_pipe(sending_pipe, message);
                        cout << "switch "<< switch_num << " sent the frame to the system " << destination << " with port " << port << endl; 
                    }
                }
            }
            close(fd);
        }
        for (int j = 0; j < new_files.size(); j++) reading_list.push_back(new_files[j]);
    }
    return 0;
}