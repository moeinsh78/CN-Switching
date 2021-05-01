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

int main(int argc, char **argv) {
    string system_num(argv[1]);
    string pipe_path(argv[2]);
    int system_number = stoi(system_num);
    cout << "New system process created -- Num: " << system_number << "\n";
    return 0;
}