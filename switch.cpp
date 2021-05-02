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

#define MAX_ROWS 256

using namespace std;

string read_message_from_pipe(string pipe_file_name);

int main(int argc, char **argv) {
    vector<string> pipe_file_names;
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
    cout << "New switch process created -- Num: " << switch_number << "\nPorts: " << number_of_ports << "\n";
    string manager_pipe = "./manager_system_" + switch_num;
    pipe_file_names.push_back(manager_pipe);
    while (1) {
        for (int i = 0; i < pipe_file_names.size(); i++) {
            string message = read_message_from_pipe(pipe_file_names[i]);
            if (message.length() < 1) 
                continue;
            else if (pipe_file_names[i] == manager_pipe) {
                stringstream s_stream(message);
                istream_iterator <string> begin(s_stream);
                istream_iterator <string> end;
                vector <string> command_tokens(begin, end);
                if (command_tokens[0] == "CONNECTED_TO_SWITCH" || command_tokens[0] == "CONNECTED_TO_SYSTEM") {
                    string reading_pipe_name = command_tokens[1];
                    char* named_pipe_path = new char[reading_pipe_name.length() + 1];
                    strcpy(named_pipe_path, reading_pipe_name.c_str());
                    mkfifo(named_pipe_path, 0666);
                    pipe_file_names.push_back(reading_pipe_name);
                    cout << "Pipe created with name: " << reading_pipe_name << "\n";
                }
            }
            else {
                // inja frame az pipe e system ba switch khunde mishe
                // dar asl "message" hamun frame emun mishe
            }

            sleep(0.001);
        }
    }
    return 0;
}

string read_message_from_pipe(string pipe_file_name) {
    char* pipe_path = new char[pipe_file_name.length() + 1];
    strcpy(pipe_path, pipe_file_name.c_str());
    int pipe = open(pipe_path, O_RDONLY);
    char msg[100];
    read(pipe, msg, 100);
    string message(msg);
     
    // pak kardane buffere pipeeeeeee????????
    
    return message;
}