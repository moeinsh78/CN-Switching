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

using namespace std;

void make_frame(int system_num, int destination, string msg) {
    unsigned char  test[] = "123456789";
    string frame;
    frame = string(system_num) + string(destination) + 
}

int main(int argc, char **argv) {
    string system_num(argv[1]);
    //int system_number = stoi(system_num);
    cout << "New system process created -- Num: " << system_num << "\n";
    string manager_pipe = "./manager_system_" + system_num;
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