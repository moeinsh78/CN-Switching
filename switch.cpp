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
    string pipe_path(argv[3]);
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
    return 0;
}