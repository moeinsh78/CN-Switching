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

#define MAX_ROWS 256

using namespace std;

int main(int argc, char **argv) {
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

    string manager_pipe = "./manager_switch_" + switch_num;
    DIR* dirp = opendir("./");
    struct dirent * pipes;
    while (1)
    {
        sleep(5);
        ofstream system_pipe(manager_pipe);
        if(!system_pipe.peek() == ifstream::traits_type::eof()) {

            /*string line;
            getline(system_pipe, line);
            system_pipe.clear();
            system_pipe.close();

            stringstream s_stream(line);
            istream_iterator <std::string> begin(s_stream);
            istream_iterator <std::string> end;
            vector <std::string> command_tokens(begin, end);

            if (command_tokens[0] == "SEND") {
                string destination = command_tokens[3];
                ifstream file(command_tokens[1]);
                while(getline(file, line)) temp = temp + "\n" + line;
                
            }*/
        }
        system_pipe.clear();
        system_pipe.close();

        sleep(5);

        while ((pipes = readdir(dirp)) != NULL){

            string pipe_name = pipe->d_name;
            string file = "./system" + system_num + "_switch_";
            if(pipe_name.find(file) == string::npos) continue;

        }
    }
    closedir(dirp);
    return 0;
}